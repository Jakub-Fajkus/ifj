#!/usr/bin/python3

'''
Testovaci skript na projekt z IFJ
Copyright (C) 2016 SsYoloSwag41 Inc.
autor: xkurak00
'''

import os,sys,re, inspect
from subprocess import Popen, PIPE

print('---IFJ test script---\n')

if len(sys.argv) != 2:
    print("ERROR: 2 arguments expected", file = sys.stderr)
    sys.exit(0)

filename = inspect.getframeinfo(inspect.currentframe()).filename
my_dir = os.path.dirname(os.path.abspath(filename)) + '/'
test_dir = my_dir + 'test/'
log_dir  = my_dir + 'logs/'
signals_path = my_dir + '.signals'
testl_path = my_dir + 'test_list.txt'

signals = {}
with open(signals_path) as input_file:
    for line in input_file:
        tmp = line[:-1].split('\t')
        tmp[1] = - int(tmp[1])
        signals[tmp[1]] = tmp[0] + ": " + tmp[2]

test_list = []
with open(testl_path) as input_file:
    for line in input_file:
        if line.startswith('#') or line == '\n':
            continue
        test_list.append(line[:-1].split('\t'))

if os.path.exists(sys.argv[1]) and os.path.isfile(sys.argv[1]):
    bin_path = os.path.abspath(sys.argv[1])
else:
    print("ERROR: argv[1] is not a file or does not exists", file = sys.stderr)
    sys.exit(0)

score = {}
score["ok"] = []
score["fail"] = []

for test in test_list:
    print("Test: \"" + test[0] + "\"", flush = True)

    test_path = test_dir + test[1]
    code_path = test_path + ".code"
    out_path  = test_path + ".out"
    in_path   = test_path + ".in"

    log_path = log_dir + test[1]
    err_save = log_path + ".stderr"
    out_save = log_path + ".stdout"

    with open(in_path) as input_file:
        input_data = bytes(input_file.read(), "UTF-8")

    real_rc = int(test[2])
    real_out = ""
    try:
        with open(out_path) as input_file:
            real_out = input_file.read()
    except:
        pass
    #cmd += "valgrind --tool=memcheck --leak-check=full "
    cmd = bin_path + " " + code_path
    process = Popen(cmd.split(' '), stdout = PIPE, stderr = PIPE, stdin  = PIPE)
    out, err = process.communicate(input = input_data, timeout = 15)

    proc_out = out.decode('utf-8')
    proc_err = err.decode('utf-8')
    proc_rc  = int(process.returncode)


    with open(err_save, 'w') as err_file,\
         open(out_save, 'w') as out_file:
        err_file.write(proc_err)
        out_file.write(proc_out)

    result = real_rc == proc_rc and real_out == proc_out

    print('  Return code:',real_rc == proc_rc)
    print('    Your return code:', proc_rc)
    print('    Real return code:', real_rc)
    print('  Stdout:', real_out == proc_out )
    print('    stdout saved: ./logs/' +  test[1] + ".stdout")
    print('    stderr saved: ./logs/' +  test[1] + ".stderr")
    if proc_rc in signals:
        print('  Signal:',signals[proc_rc])
    if result:
        print('  Test OK\n')
        score["ok"].append(test[0])
    else:
        print('  Test FAILED\n')
        score["fail"].append(test[0])

print("DONE, ok:", len(score["ok"]), "failed:", len(score["fail"]) )
if score["fail"]:
    print('Failed tests:')
    for i in score["fail"]:
        print(" ",i)
    sys.exit(1)
