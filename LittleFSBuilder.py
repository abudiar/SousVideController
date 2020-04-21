Import("env")
import os
cwd = os.getcwd()
env.Replace(MKSPIFFSTOOL=cwd+'mklittlefs')
