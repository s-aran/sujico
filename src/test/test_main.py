# -*- coding: utf-8 -*-

import sys
import pytest
import subprocess
import os
import os.path
import shutil
import shlex
import platform
import re
import datetime
import stat

DefaultConfName = '.sujiconf.toml'

def is_windows():
    return platform.system() == 'Windows'

MainProgram='suji.exe' if is_windows() else 'suji'
SourcePath = os.path.join(R'../x64/Release/' if is_windows() else R'../piyo/', MainProgram)
BasePath = os.path.join('bin', MainProgram)

def execute(cmdline):
    is_win = is_windows()
    enc = 'shiftjis' if is_win else 'utf-8'
    sl = shlex.split(cmdline if is_win else shlex.quote(cmdline), posix=not is_win)
    p = subprocess.Popen(sl, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding=enc)

    p.wait()

    try:
        out, err = p.communicate()
    except IndexError:
        print('Index error in p.communicate()')
        out = ''
        err = ''

    ret = p.returncode

    return ret, out.strip(), err.strip()

     
def assert_exec_res(cmdlines, expected_r, expected_o, expected_e, show_r=False, show_o=False, show_e=False):
    r, o, e = execute(' '.join(cmdlines) if type(cmdlines) == list else cmdlines)

    if show_r:
        print('======== return code ========')
        print(r)
    if show_o:
        print('======== stdout ========')
        print(o)
    if show_e:
        print('======== stderr ========')
        print(e)

    assert r          == expected_r
    assert o.rstrip() == expected_o
    assert e.rstrip() == expected_e

def list2string(l):
    return '\n'.join(l)

def string2list(s):
    return s.split(os.linesep)


def generate_toml(kvdic, fmt, file=DefaultConfName):
    toml_lines = []
    for k, v in kvdic.items():
        if type(v) == int:
            l = '{k} = {v}'.format(k = k, v = v)
        elif type(v) == str:
            l = '{k} = "{v}"'.format(k = k, v = v)
        toml_lines.append(l)
    toml_lines.append('')
    toml_lines.append('format = \'\'\'' + fmt + '\'\'\'')
        
    with open(file, 'w', encoding='utf-8') as f:
        toml = list2string(toml_lines)
        f.writelines(toml)


class TestSuji(object):
    @classmethod
    def setup_class(cls):
        cls.git_testdir = './gittest/'
        if os.path.exists(cls.git_testdir):
            shutil.rmtree(cls.git_testdir, ignore_errors=False, onerror=lambda f, p, e: (os.chmod(p, stat.S_IWRITE), f(p)))

        if os.path.exists(BasePath):
            shutil.rmtree('bin')

        if not os.path.exists(BasePath):
            os.makedirs(os.path.dirname(BasePath))
            shutil.copyfile(SourcePath, BasePath)
            os.chmod(BasePath, 0o775)

        print('setup')

    @classmethod
    def teardown_class(cls):
        print('teardown')

    def setup_method(self, method):
        print('setup_method')
        print(method)
        if os.path.exists(DefaultConfName):
            os.remove(DefaultConfName)
        if os.path.exists('another.toml'):
            os.remove('another.toml')

    def teardown_method(self, method):
        print('teardown_method')
        print(method)

    def test_selftest(self):
        assert_exec_res('echo hoge', 0, 'hoge', '')

    def test_version(self):
        cmdlines = [
            BasePath,
            '-v',
            ]

        r, o, e = execute(' '.join(cmdlines))
        assert r == 0 
        assert re.match('^sujico \(suji command\) version: .*', o.rstrip()) is not None
        assert e == ''

    def test_missing_default_conf(self):
        if os.path.exists(DefaultConfName):
            os.remove(DefaultConfName)

        assert_exec_res(BasePath, 
                        1, 
                        '',
                        '''file read error: .sujiconf.toml
toml parse error''')

    def test_normal(self):
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        assert_exec_res(BasePath, 0, '1.00', '')

    def test_another_conf(self):
        fmt = '${03:ZERO}'
        generate_toml({
            'ZERO': 0,
            }, fmt, 'another.toml')
        
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-f',
            'another.toml',
            ]

        assert_exec_res(cmdlines, 0, '000', '')

    def test_help(self):
        cmdlines = [
            BasePath,
            '-h',
            ]

        r, o, e = execute(' '.join(cmdlines))
        assert r == 0 
        assert re.match('^sujico \(suji command\) options:.*', o.rstrip()) is not None
        assert e == ''


    def test_increment(self):
        fmt = '${Major}.${02:Minor}'

        # integer
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-i',
            'Major'
            ]
        
        assert_exec_res(cmdlines, 0, '2.00', '')
        assert_exec_res(BasePath, 0, '1.00', '')
        
        # string
        generate_toml({
            'Major': 'a',
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-i',
            'Major'
            ]
        
        assert_exec_res(cmdlines, 0, 'b.00', '')
        assert_exec_res(BasePath, 0, 'a.00', '')


        generate_toml({
            'Major': 'B',
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-i',
            'Major'
            ]
        
        assert_exec_res(cmdlines, 0, 'C.00', '')
        assert_exec_res(BasePath, 0, 'B.00', '')




    def test_set(self):
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-s',
            'Major=3',
            ]

        assert_exec_res(cmdlines, 0, '3.00', '')
        assert_exec_res(BasePath, 0, '1.00', '')


    def test_write(self):
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-s',
            'Major=3',
            '-w',
            ]

        assert_exec_res(cmdlines, 0, '3.00', '')
        assert_exec_res(BasePath, 0, '3.00', '')


    def test_write_another_conf(self):
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-s',
            'Major=3',
            '-w',
            'another.toml'
            ]

        assert_exec_res(cmdlines, 0, '3.00', '')
        assert_exec_res(BasePath, 0, '1.00', '')

        cmdlines = [
            BasePath,
            '-f',
            'another.toml'
            ]

        assert_exec_res(cmdlines, 0, '3.00', '')


    def test_invalid_key(self):
        fmt = '${Meja-}.${02:Maina-}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        assert_exec_res(BasePath, 0, fmt, '')


    def test_increment_invalid_key(self):
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-i',
            'Meja-'
            ]
        
        assert_exec_res(cmdlines, 1, '', 'Meja- is undefined')
        assert_exec_res(BasePath, 0, '1.00', '')


    def test_set_invalid_key(self):
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)

        cmdlines = [
            BasePath,
            '-s',
            'Meja-=3',
            ]

        assert_exec_res(cmdlines, 1, '', 'Meja- is undefined')
        assert_exec_res(BasePath, 0, '1.00', '')


    def test_increment_wrong_tyoe(self):
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': '@',
            'Minor': '0',
            }, fmt)
        
        cmdlines = [
            BasePath,
            '-i',
            'Major'
            ]

        assert_exec_res(cmdlines, 1, '', 'cannot increment: "@"')
        assert_exec_res(BasePath, 0, '@.00', '')
        

    def test_set_wrong_type(self):
        fmt = '${Major}.${02:Minor}'
        generate_toml({
            'Major': 1,
            'Minor': 0,
            }, fmt)
        
        cmdlines = [
            BasePath,
            '-s',
            'Major=a'
            ]

        assert_exec_res(cmdlines, 1, '', 'a is wrong type')
        assert_exec_res(BasePath, 0, '1.00', '')
        

    def test_increment_max(self):
        max_int = '18446744073709551615'
        fmt = '${Major}.${02:Minor}'

        generate_toml({
            'Major': max_int,
            'Minor': 0,
            }, fmt)
        
        cmdlines = [
            BasePath,
            '-i',
            'Major'
            ]

        assert_exec_res(cmdlines, 1, '', 'cannot increment: {0}'.format(max_int))
        assert_exec_res(BasePath, 0, '{0}.00'.format(max_int), '')

        generate_toml({
            'Major': 'z',
            'Minor': '0',
            }, fmt)
        
        cmdlines = [
            BasePath,
            '-i',
            'Major'
            ]

        assert_exec_res(cmdlines, 1, '', 'cannot increment: z')
        assert_exec_res(BasePath, 0, 'z.00', '')


        generate_toml({
            'Major': 'Z',
            'Minor': '0',
            }, fmt)
        
        cmdlines = [
            BasePath,
            '-i',
            'Major'
            ]

        assert_exec_res(cmdlines, 1, '', 'cannot increment: Z')
        assert_exec_res(BasePath, 0, 'Z.00', '')


    def test_increment_negative(self):
        fmt = '${Major}.${02:Minor}'

        generate_toml({
            'Major': -2,
            'Minor': 0,
            }, fmt)
        
        cmdlines = [
            BasePath,
            '-i',
            'Major'
            ]

        assert_exec_res(cmdlines, 1, '', 'cannot increment: -2')
        assert_exec_res(BasePath, 0, '-2.00', '')
        

    def test_hg(self):
        os.environ['LANGUAGE'] = 'en_US.UTF-8'
        r, o, e = execute('hg log -l 1 -b .')
        
        d = {}
        m = re.search(R'^changeset: +([0-9]+?):([a-z0-9]+?)$', o, re.MULTILINE)
        if m is not None:
            d['changeset'] = m[1]
            d['hash'] = m[2]

        m = re.search(R'^tag: +(.*)$', o, re.MULTILINE)
        if m is not None:
            d['tag'] = m[1]

        m = re.search(R'^user: +(.*?) <(.*?)>$', o, re.MULTILINE)
        if m is not None:
            d['user'] = m[1]
            d['mail'] = m[2]

        m = re.search(R'^date: +.+? (.+? [0-9]+ [0-9]+?:[0-9]+?:[0-9]+? [0-9]+? [+-]?[0-9]+?)$', o, re.MULTILINE)
        if m is not None:
            d['datetime'] = m[1]

        dt = datetime.datetime.strptime(d['datetime'], '%b %d %H:%M:%S %Y %z')
        
        d['date'] = dt.strftime('%Y/%m/%d')
        d['time'] = dt.strftime('%H:%M:%S')

        d['utcdate'] = dt.astimezone(datetime.timezone.utc).strftime('%Y/%m/%d')
        d['utctime'] = dt.astimezone(datetime.timezone.utc).strftime('%H:%M:%S')
                       
        # fmt = '${hg:changeset},${hg:revision},${hg:hash},${hg:longhash},${hg:date},${hg:time},${hg:utcdate},${hg:utctime},${hg:commitlog},${hg:tag},${branch}'
        fmt = 'hg,${hg:changeset},${hg:revision},${hg:hash},${hg:date},${hg:time},${hg:utcdate},${hg:utctime},${hg:tag},${hg:changeset}'
        generate_toml({}, fmt)
        
        expected = '{0},{1},{2},{3},{4},{5},{6},{7},{8},{9}'.format(
            'hg',
            d['changeset'],
            d['changeset'],
            d['hash'],
            d['date'],
            d['time'],
            d['utcdate'],
            d['utctime'],
            d['tag'],
            d['changeset'],
            )

        assert_exec_res(BasePath, 0, expected, '')
        

    def test_git(self):
        os.makedirs(TestSuji.git_testdir)

        old_cd = os.getcwd()
        os.chdir(TestSuji.git_testdir)

        execute('git init .')
        execute('git config --local user.name "John Doe"')
        execute('git config --local user.email "john.doe@localhost.local"')

        tmp_name = 'test'
        with open(tmp_name, 'w') as f:
            f.write('testtest')

        execute('git add {0}'.format(tmp_name))
        execute('git commit -m "initial commit"')

        r, o, e = execute('git rev-parse HEAD')
        longhash = o.rstrip()
        r, o, e = execute('git rev-parse --short HEAD')
        shorthash = o.rstrip()
        

        fmt = '${git:hash},${git:longhash}'
        generate_toml({}, fmt)
        
        cmdlines = [
            os.path.join('..', BasePath),
            ]

        expected  = '{0},{1}'.format(shorthash, longhash)
        assert_exec_res(cmdlines, 0, expected, '', show_r = True, show_o=True, show_e=True)
         
   