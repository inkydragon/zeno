@echo off
@rem get the cpython.zip and extract it here..
zenv\PCBuild\amd64\python -m ensurepip
python -m pip install PyQt5 numpy -t zenv\Lib\site-packages -i https://mirrors.aliyun.com/pypi/simple
zenv\PCBuild\amd64\python setup.py install
copy /y scripts\start.bat zenv
copy /y scripts\start_nocons.bat zenv