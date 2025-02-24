import sys
import os
import re

def remove_comments(source):
    """// コメントや /* */ コメントを削除する"""
    source = re.sub(r'//.*', '', source)  # 行コメントの削除
    source = re.sub(r'/\*[\s\S]*?\*/', '', source)  # ブロックコメントの削除
    return source

def main(args):
    # コマンドライン引数の処理
    print(f'[generate_enum_info.py]NumArgs=={args.__len__()}')
    if len(args) < 2 or len(args) > 3:
        print(f"Usage: 'python script.py <source-path> <generated-path> [encoding]'")
        return

    source_path = args[0]
    dest_path = args[1]  # <generated-path> をそのまま使用

    # デフォルトエンコーディングをUTF-8に設定
    encoding = 'utf-8'
    if len(args) == 3:
        encoding = args[2]

    # 相対パスの取得 (dest_pathからsource_pathへの相対パス)
    try:
        relative_include_path = os.path.relpath(source_path, os.path.dirname(dest_path))
    except Exception as e:
        print(f"Error calculating relative path: {e}")
        return

    # ファイルを読み込む
    try:
        with open(source_path, 'r', encoding=encoding) as source_file:
            source_content = source_file.read()
    except Exception as e:
        print(f"Error reading source file: {e}")
        return

    # コメントを除去する
    source_content = remove_comments(source_content)

    # 列挙型と列挙子を保持する辞書
    enum_class_name_to_enumerator_value_pair = {}

    # DEFINE_ENUM() または改行や空白が含まれる場合にも対応する正規表現
    pattern = r'^\s*DEFINE_ENUM\s*\(\s*\)\s*enum\s+class\s+(\w+)\s*(?:\:\s*[\w\s]+)?\s*\{([\s\S]*?)\}\s*;'
    matches = re.findall(pattern, source_content, re.DOTALL | re.MULTILINE)

    # 列挙型ごとに列挙子を解析
    for match in matches:
        enum_name = match[0]  # enum class 名
        enum_body = match[1]  # enum class の中身

        enumerators = []

        # 列挙子をカンマで分割
        enumerator_definitions = re.split(r',\s*', enum_body.strip())

        for enumerator_def in enumerator_definitions:
            enumerator = enumerator_def.strip()
            if not enumerator:
                continue

            # 値の指定は無視し、列挙子名のみを抽出
            if '=' in enumerator:
                enumerator = enumerator.split('=')[0].strip()

            enumerators.append(enumerator)

        # 辞書に追加
        enum_class_name_to_enumerator_value_pair[enum_name] = enumerators

    # 結果をファイルに書き込む
    try:
        with open(dest_path, 'w', encoding=encoding) as dest_file:
            # インクルード文（相対パスを使ってソースファイルをインクルード）
            dest_file.write(f'#include "{relative_include_path}"\n\n')

            for enum_name, enumerators in enum_class_name_to_enumerator_value_pair.items():
                # std::vector の生成（単に列挙子を並べる）
                dest_file.write(f'std::vector<{enum_name}> EnumInfo<{enum_name}>::enumerators =\n')
                dest_file.write('{\n')
                for enumerator in enumerators:
                    dest_file.write(f'    {enum_name}::{enumerator},\n')
                dest_file.write('};\n\n')

                # std::unordered_map の生成
                dest_file.write(f'std::unordered_map<std::string, {enum_name}> EnumInfo<{enum_name}>::name_to_enum_map =\n')
                dest_file.write('{\n')
                for enumerator in enumerators:
                    dest_file.write(f'    {{"{enumerator}", {enum_name}::{enumerator}}},\n')
                dest_file.write('};\n\n')

        print(f"[generate_enum_info.py]Generated file: {dest_path}")

    except Exception as e:
        print(f"Error writing generated file: {e}")

if __name__ == "__main__":
    main(sys.argv[1:])
