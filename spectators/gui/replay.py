import argparse
import gzip
import logging

from state import DumpReader
from server import Server

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser(description="Replay a stechec match from "
                                                 "a dump file")
    parser.add_argument('dump', metavar='dump-file', type=argparse.FileType('r'),
                        help="The dump file (plain JSON or gzipped JSON)")
    parser.add_argument('-a', '--host', help="Listen address",
                        default='127.0.0.1')
    parser.add_argument('-p', '--port', type=int, help="Listen port",
                        default=8099)
    parser.add_argument('-t', '--tv', action='store_true', help="TV mode")

    args = parser.parse_args()

    if args.dump.name.endswith('.gz'):
        args.dump.close()
        dump = gzip.open(args.dump.name, 'rt')
    else:
        dump = args.dump

    reader = DumpReader(dump)
    server = Server(reader, (args.host, args.port), tv_mode=args.tv)
    server.start()
