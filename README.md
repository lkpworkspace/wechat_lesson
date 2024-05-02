# wechat_lesson
wechat 公众号后台模块

# 依赖
```sh
sudo apt-get install libpugixml-dev
```

# 模块安装
```sh
git clone https://github.com/lkpworkspace/wechat_lesson.git
cd wechat_lesson
cmake -S . -B build
cmake --build build -j --config Release --target install
```