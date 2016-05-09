import logging

from state import StechecReader
from server import Server


logging.basicConfig(level=logging.DEBUG)

reader = StechecReader()
server = Server(reader, ('127.0.0.1', 8099), tv_mode=False)


def partie_init():
    server.start()


def jouer_tour():
    reader.put_state()


def partie_fin():
    reader.do_end()
    reader.do_gui_end()
    server.end_game()
    reader.wait_gui_end()
