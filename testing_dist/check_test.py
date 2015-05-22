#!/usr/bin/env python
import os
import argparse
import subprocess
import base64
import json

def get_size(start_path = '.'):
  total_size = 0
  for dirpath, dirnames, filenames in os.walk(start_path):
    for f in filenames:
      fp = os.path.join(dirpath, f)
      total_size += os.path.getsize(fp)
  return total_size

def main(testin, prefix, keeplog):
  t = json.load(file(testin, 'r'))
  #write out the batch file, if present
  if t.has_key('batch'):
    buf = t['batch']
    buf = base64.b64decode(buf)
    bufout = file('batch', 'w')
    bufout.write(buf)
    bufout.close()
  #run all of the commands and test their outputs
  cmds = t['tests']
  passed = True
  for i in cmds:
    inpt = i['input']
    args = inpt.split(" ")
    cmd = args[0]
    cmd = prefix + cmd
    args[0] = cmd
    print "running command %s" % inpt
    p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out,err = p.communicate()
    ret = p.returncode
    #check and see if out,err,ret match what is in this test
    expectedout = ""
    expectederr = ""
    expectedret = 0
    if i.has_key('output'):
      expectedout = i['output']
    if i.has_key('error'):
      expectederr = i['error']
    if i.has_key('exit'):
      expectedret = i['exit']
    out = out.replace("\n", "").replace("\r", "").replace(" ", "")
    err = err.replace("\n", "").replace("\r", "").replace(" ", "")
    expectedout = expectedout.replace("\n", "").replace("\r", "").replace(" ", "")
    expectederr = expectederr.replace("\n", "").replace("\r", "").replace(" ", "")
    if out != expectedout:
      print "got %s expected %s" % (out, expectedout)
      passed = False
    if err != expectederr:
      print "got %s expected %s" % (err, expectederr)
      passed = False
    if ret != expectedret:
      print "got %d expected %d" % (ret, expectedret)
      passed = False
  if passed == True:
    print "Test passed"

if __name__ =='__main__':
  parser = argparse.ArgumentParser(description='Test executor')
  parser.add_argument('--prefix', dest='prefix', type=str, default=".",
                    help='program prefix')
  parser.add_argument('--test', dest='test', type=str, default="test.json", required=True,
                    help='test input')
  parser.add_argument('--keep-logfile', dest='keeplog', type=bool, default=False,
                    help='do not auto-erase the log file output')

  args = parser.parse_args()
  main(args.test, args.prefix, args.keeplog)
