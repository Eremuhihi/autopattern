# AutoPattern
セルベースのインタラクティブな自動で動く模様を作成・保存するためのリポジトリです。

## 概要
AutoPattern は、インタラクティブなセルベースの自動で動く模様を収録していくための場所です。bgfx と SDL2 を用いて、Linux、Windows、macOS といった複数のプラットフォームで描画できるようにしています。

## インストール
### 前提条件

-   Git (2.30+)
-   CMake (3.16+)
-   C++20 対応コンパイラ
    -   Linux: GCC 11+ または Clang 14+
    -   Windows: MSVC 2022 (Visual Studio または Build Tools)
    -   macOS: Xcode 14+ (Apple Clang)
-   （推奨）Ninja ビルドシステム
-   vcpkg（SDL2 や bgfx の依存関係を CMake toolchain として使用）

bgfx のソースコードは external/bgfx.cmake に配置されていることを想定しています（リポジトリに含まれているか、サブモジュールとして追加）。

### Windows

```
vcpkg install sdl2
git clone https://github.com/Eremuhihi/autopattern.git autopattern
cd autopattern
git submodule update --init --recursive

mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" `
  -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:[PATH TO YOUR VCPKG]\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release

.\Release\autopattern.exe
```

Windows では Visual Studio 2022 と vcpkg が事前にインストールされている必要があります。

### Linux

```
sudo apt install libsdl2-dev pkg-config
git clone https://github.com/Eremuhihi/autopattern.git autopattern
cd autopattern
git submodule update --init --recursive

mkdir build && cd build
cmake ..
cmake --build .

./autopattern
```

Linux では、最新の GCC または Clang、make または ninja、pkg-config を含む標準的な開発環境が必要です。SDL2 の開発ヘッダも利用可能である必要があります。

### macOS

```
brew install sdl2
git clone https://github.com/Eremuhihi/autopattern.git autopattern
cd autopattern
git submodule update --init --recursive

mkdir build && cd build
cmake ..
cmake --build .

./autopattern
```

macOS では Xcode および Apple のコマンドラインツール、依存関係管理のために Homebrew が必要です。

## サンプル
| 名前 | 説明 | 動画 |
|------|------|------|
| noise | 最もシンプルな例として作成された、ノイズによる模様です。 | ![noise pattern](docs/assets/videos/noise.gif) |

## ライセンス
このプロジェクトは MIT License の下でライセンスされています。詳細は [LICENSE](./LICENSE) ファイルを参照してください。

## 謝辞
- [bgfx](https://github.com/bkaradzic/bgfx)
- [SDL2](https://github.com/libsdl-org/SDL)

## リンク
[X](https://x.com/eremuhihi)
