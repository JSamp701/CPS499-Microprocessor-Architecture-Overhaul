from ctypes import *

def main():
	testlib = cdll.LoadLibrary('/home/user/Desktop/ctypestest/mysotest.so')
	testlib.sayhi()
	teststr = c_char_p(b"Joel")
	testlib.greet(teststr)

main()
	