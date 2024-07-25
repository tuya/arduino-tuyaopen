#coding=utf-8

import os

class FileLock:
    def __init__(self, lockName, path=os.getcwd()):
        self.lockName = lockName
        self.path = path

    def lock(self):
        # 如果文件存在，说明已经被锁定
        try:
            self.lockFile = open(self.lockName, 'w')
            self.lockFile.write('lock')
            self.lockFile.flush()
            return True
        except Exception as e:
            print('lock file failed: ' + str(e))
            return False

    def unlock(self):
        try:
            self.lockFile.close()
            os.remove(self.lockName)
            return True
        except Exception as e:
            print('unlock file failed: ' + str(e))
            return False

    def isLocked(self):
        return os.path.exists(self.lockName)


