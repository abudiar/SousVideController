from distutils.dir_util import copy_tree
from pathlib import Path
import pathlib
import shutil
import os

# copy subdirectory example


source_dir = "webpage"
dest_dir = "data"

if os.path.exists(dest_dir):
    shutil.rmtree(dest_dir)


copy_tree(source_dir, dest_dir)

# for path in Path(dest_dir).rglob('*'):
#     if not 'jquery.' in path.name:
#         # print(Path(path.name).absolute())
#         # print('GZipping ' + pathlib.Path(path.name).absolute())
#         # print(os.path.relpath(path))
#         os.system('gzip ' + '"' + os.path.relpath(path) + '"')
#     if 'jquery.' in path.name:
#         print("Jquery File: " + os.path.relpath(path) + " -- ignored")

# for path in Path(dest_dir).rglob('*'):
#     print(path.name)

os.system('gzip -r ' + dest_dir + '\*')
os.system(r'C:\Users\alanw\.platformio\penv\Scripts\platformio.exe run --target uploadfs')
