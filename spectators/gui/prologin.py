import asyncio
import ctypes
import json
import logging
import os.path
import threading

from aiohttp import web
from aiohttp.websocket import Message

import api

LIBPROLOGIN = 'libprologin2016.so'

logger = logging.Logger(__name__)
root = os.path.abspath(os.path.dirname(__file__))


class Server:
    def __init__(self):
        self.event_ready = threading.Event()

        librules = ctypes.cdll.LoadLibrary(LIBPROLOGIN)
        self.get_dump = librules.dump_state_json
        self.get_dump.argtypes = []
        self.get_dump.restype = ctypes.c_char_p

        self.loop = asyncio.new_event_loop()
        self.queue = asyncio.Queue(loop=self.loop)
        self.app = web.Application(loop=self.loop)
        self.app.on_shutdown.append(self.on_shutdown)
        self.app.router.add_static('/www', os.path.join(root, 'www'))
        self.app.router.add_route('GET', '/ws', self.websocket_handler)
        self.ws = web.WebSocketResponse()

        self.run_thread = threading.Thread(target=self.run_forever)

    async def websocket_handler(self, request):
        if self.ws.prepared:
            logger.warning("session was already opened")
            await self.ws.close(code=998, message="session already opened")
        else:
            logger.info("new WS session")

        self.ws = web.WebSocketResponse()

        await self.ws.prepare(request)
        async for msg in self.ws:
            self.handle_message(msg)

        return self.ws

    async def on_shutdown(self, *args):
        if self.ws.prepared and not self.ws.closed:
            await self.ws.close(code=999, message="Server shutdown")

    def send(self, cmd, **kwargs):
        msg = {}
        msg.update(kwargs)
        msg['c'] = cmd
        self.ws.send_str(json.dumps(msg))

    def handle_message(self, msg: Message):
        data = json.loads(msg.data)
        cmd = data['c']
        print("received WS message:", data)
        if cmd == "hello":
            self.send("whatsup", size=api.TAILLE_TERRAIN, maxTurn=api.NB_TOURS)
            return
        if cmd == "ready":
            self.event_ready.set()

    def start(self):
        logger.info("start")
        self.run_thread.start()
        print("Waiting for browser")
        # webbrowser.open('http://127.0.0.1:8099/www/index.html')
        self.event_ready.wait()

    def run_forever(self):
        asyncio.set_event_loop(self.loop)
        handler = self.app.make_handler()
        srv = self.loop.run_until_complete(self.loop.create_server(
            handler, '127.0.0.1', 8099))
        try:
            self.loop.run_forever()
        except KeyboardInterrupt:
            pass
        finally:
            self.loop.run_until_complete(srv.wait_closed())
            self.loop.run_until_complete(self.app.shutdown())
            self.loop.run_until_complete(handler.finish_connections(4))
            self.loop.run_until_complete(self.app.cleanup())

    def next_turn(self):
        print("next_turn")
        state = json.loads(self.get_dump().decode())

        self.send('turn', state=state)
        self.event_ready.clear()
        self.event_ready.wait()

    def end_game(self):
        print("end_game")
        self.send('end')
        self.ws.close(code=1)


logging.basicConfig(level=logging.DEBUG)
server = Server()


def partie_init():
    server.start()


def jouer_tour():
    server.next_turn()


def partie_fin():
    server.end_game()
