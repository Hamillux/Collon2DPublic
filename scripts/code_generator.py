"""
カスタムビルドツールのコマンドから呼び出すスクリプト
Syntax: python code_generator.py <source> <generated> [encoding]
"""

import sys
import xml.etree.ElementTree as ET
import os
from CodeGeneratingOperations import *

# 定数
ROOT_TAG = "GenerationConfig"
SOURCE_ATTRIB = "source"

def get_gen_file_name(source_path):
    splited = os.path.splitext(source_path)
    return splited[0] + ".gen.xml"

# ノード名と, 対応する処理を行う関数のマップ
OPERATION_NODE_HANDLER_MAP = {
    'ReplaceMacro': replace_macro,
}

def has_all_operation_node_as_children(gen_config_node: ET.Element, out_not_existing_tags:list[str]) -> bool:
    out_not_existing_tags.clear()
    for op_tag in OPERATION_NODE_HANDLER_MAP.keys():
        if(gen_config_node.find(op_tag) == None):
            out_not_existing_tags.append(op_tag)
    return out_not_existing_tags.__len__() == 0
            
def process_file(source_path:str, generation_config_path:str, generated_path:str, file_encoding:str) -> bool:
    try:
        if not os.path.exists(source_path):
            print_message(source_path, 0, MessageKind.ERROR, f"The source file '{source_path}' does not exist.")
            return False
        
        # Copy source file content to generated file.
        with open(source_path, 'r', encoding=file_encoding) as f:
            generated_file_content:str = f.read()
    except (FileNotFoundError, UnicodeDecodeError, IOError) as e:
        print_message(source_path, 0, MessageKind.ERROR, f"Could not read source file '{source_path}'. {e}")
        return False
    except (LookupError) as e:
        print_message(source_path, 0, MessageKind.ERROR, f"{e}")
        return False

    try:
        # Parse and validate generation config xml
        tree = ET.parse(generation_config_path)
        root = tree.getroot()

        if root.tag != ROOT_TAG:
            print_message(source_path, 0, MessageKind.ERROR, f"Root node is not '{ROOT_TAG}', found '{root.tag}' instead.")
            return False
        # TODO: delete. Filename.gen.xmlはFilename.cppと1対1対応させるのでSOURCE_ATTRIB属性は不要になる
        # if root.get(SOURCE_ATTRIB) != os.path.basename(source_path):
        #     print_message(generation_config_path, 0, MessageKind.ERROR, f"'{SOURCE_ATTRIB}' attribute of '{root.tag}' does not match source file name '{os.path.basename(source_path)}'.")
        #     return False

    except (FileNotFoundError, ET.ParseError, IOError) as e:
        print_message(source_path, 0, MessageKind.ERROR, f"Could not read XML file '{generation_config_path}'. {e}")
        return False
    
    # ルートが必要な操作タグを持っているかチェック.
    not_existing_tags:list[str] = []
    if not has_all_operation_node_as_children(root, not_existing_tags):
        print_message(generation_config_path, 0, MessageKind.ERROR, f"'{root.tag}' does not have children: {', '.join(not_existing_tags)}")
        sys.exit(1)

    # 各種操作を実行
    all_process_succeeded:bool = True
    for operation_node in list(root):  # XMLを走査
        handler = OPERATION_NODE_HANDLER_MAP.get(operation_node.tag)
        if handler:
            generated_file_content:str = handler(operation_node, generated_file_content, source_path)
            if generated_file_content is None:
                all_process_succeeded = False
                break;

    # out_fileに結果を書き込む
    if all_process_succeeded:
        try:
            with open(generated_path, 'w', encoding=file_encoding) as f:
                f.write(generated_file_content)
        except IOError as e:
            print_message(generated_path, 0, MessageKind.ERROR, f"Could not write to output file '{generated_path}'. {e}")
            all_process_succeeded = False
        except Exception as e:
            print_message(print_message(generated_path, 0, MessageKind.ERROR, f"Could not write to output file '{generated_path}'. {e}"))

    # 不要なタグが削除されたXMLをファイルに書き込む
    tree.write(generation_config_path, encoding='utf-8', xml_declaration=True)
    return all_process_succeeded

def main(args:list[str]):
    if len(args) < 3:
        print_message("script.py", 0, MessageKind.ERROR, "Invalid arguments. Usage: python *.py <SourcePath> <GeneratedPath> [encoding]")
        sys.exit(1)

    source_path = args[1]
    generated_path = args[2]
    encoding = "utf-8"

    if len(args) > 3:
        encoding = args[3]

    generation_config_path = get_gen_file_name(source_path)

    sys.stdout.write(f"[code_generator.py] Source='{source_path}' Generated='{generated_path}' Encoding='{encoding}' GenerationConfig='{generation_config_path}'\n")
    ret = process_file(source_path, generation_config_path, generated_path, encoding)
    if ret:
        sys.exit(0)
    else:
        sys.stdout.write(f"[code_generator.py] Generation failed: {source_path}")
        sys.exit(1)

if __name__ == "__main__":
    main(sys.argv)