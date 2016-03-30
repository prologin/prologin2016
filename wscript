#!/usr/bin/env python3

import glob
import os.path


def options(opt):
    pass

def configure(cfg):
    pass

def build(bld):
    bld.shlib(
        source = '''
            src/action_ameliorer.cc
            src/action_construire.cc
            src/action_deplacer_aspiration.cc
            src/action_detruire.cc
            src/api.cc
            src/entry.cc
            src/game_state.cc
            src/interface.cc
            src/rules.cc
        ''',
        defines = ['MODULE_COLOR=ANSI_COL_BROWN', 'MODULE_NAME="rules"'],
        target = 'prologin2016',
        use = ['stechec2'],
    )

    abs_pattern = os.path.join(bld.path.abspath(), 'src/tests/test-*.cc')
    for test_src in glob.glob(abs_pattern):

        test_name = os.path.split(test_src)[-1]
        test_name = test_name[5:-3]

        # Waf requires a relative path for the source
        src_relpath = os.path.relpath(test_src, bld.path.abspath())

        bld.program(
            features = 'gtest',
            source = src_relpath,
            target = 'prologin2016-test-{}'.format(test_name),
            use = ['prologin2016', 'stechec2-utils'],
            includes = ['.'],
            defines = ['MODULE_COLOR=ANSI_COL_PURPLE',
                       'MODULE_NAME="prologin2016"'],
        )

    bld.install_files('${PREFIX}/share/stechec2/prologin2016', [
        'prologin2016.yml',
    ])
