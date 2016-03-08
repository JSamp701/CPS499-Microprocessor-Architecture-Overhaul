class QuickLogger:
    def log(self,astring):
        self._logger(astring)

    def _tostdout(self, astring):
        print(astring)

    def _nothing(self):
        # print("QuickLogger cleanned up")
        self._closer = self._nothing

    def _tofile(self, astring):
        self._file.write(astring + "\n")
        self._file.flush()
        #print("got here")

    def _closefile(self):
        self._file.flush()
        self._file.close()
        # print("QuickLogger cleanned up")

    def __init__(self, filename):
        if(filename is not None):
            self._filename=filename
            self._file = open(filename, 'w')
            self._logger = self._tofile
            self._closer = self._closefile
        else:
            self._logger = self._tostdout
            self._closer = self._nothing

    def cleanup(self):
        self._closer()