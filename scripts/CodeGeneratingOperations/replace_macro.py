from CodeGeneratingOperations.code_generation_utility import *
import re

def hello_replace_macro():
    print("Hello, from replace_macro.py")

    # ReplaceMacroノードの処理
"""
replaceノードのname属性によってソースファイル中のマクロを特定し,
value属性の値と置き換える
"""
def replace_macro(replace_macro_node: ET.Element, generated_source_content:str, source_file:str) -> str|None:
    macro_map = {
    'CLN2D_GEN_REPLACE_STRING': (lambda v: f'"{v}"', lambda v: isinstance(v, str)),
    'CLN2D_GEN_REPLACE_CHAR': (lambda v: f"'{v}'", lambda v: isinstance(v, str) and len(v) == 1),
    'CLN2D_GEN_REPLACE_INT': (lambda v: f'{v}', lambda v: v.lstrip('-').isdigit()), 
    'CLN2D_GEN_REPLACE_FLOAT': (lambda v: f'{v}', lambda v: re.match(r'^-?\d+(\.\d+)?$', v) is not None), 
    'CLN2D_GEN_REPLACE_BOOL': (lambda v: f'{v}', lambda v: v in ['true', 'false']),
    }


    # CLN2D_GEN_REPLACE_XXX(YYY)
    # YYY consists of non-white-space characters
    # Capture group
    # 1(XXX): STRING, CHAR, INT, ...
    # 2(YYY): replace-name: value of 'name' attribute in 'replace' node
    VALID_MACRO_PATTERN = r'CLN2D_GEN_REPLACE_(.*)\(([^\s]+)\)'
    
    remove_node_list = []
    for child_node in list(replace_macro_node):
        if child_node.tag != 'replace':
            remove_node_list.append(child_node)
            continue

        replace_name = child_node.get('name')
        replace_value = child_node.get('value')

        if replace_name is None or replace_value is None:
            remove_node_list.append(child_node)
            continue


        found = False
        for macro_type, (formatter, validator) in macro_map.items():
            # ダブルクオートなしのパターン
            search_str_no_quotes = f'{macro_type}({replace_name})'
            # ダブルクオート付きのパターン
            search_str_with_quotes = f'{macro_type}("{replace_name}")'

            # ダブルクオートなしのパターンに一致した場合
            if search_str_no_quotes in generated_source_content:
                if not validator(replace_value):
                    line_number = generated_source_content[:generated_source_content.find(search_str_no_quotes)].count('\n') + 1
                    print_message(source_file, line_number, MessageKind.ERROR, f"Invalid value '{replace_value}' for '{search_str_no_quotes}'.")
                    return None

                generated_source_content = generated_source_content.replace(search_str_no_quotes, formatter(replace_value))
                found = True
                break

            # ダブルクオート付きのパターンに一致した場合
            elif search_str_with_quotes in generated_source_content:
                if not validator(replace_value):
                    line_number = generated_source_content[:generated_source_content.find(search_str_with_quotes)].count('\n') + 1
                    print_message(source_file, line_number, MessageKind.ERROR, f"Invalid value '{replace_value}' for '{search_str_with_quotes}'.")
                    return None

                generated_source_content = generated_source_content.replace(search_str_with_quotes, formatter(replace_value))
                found = True
                break
        if not found:
            remove_node_list.append(child_node)

    remove_children_nodes(replace_macro_node, remove_node_list)

    unreplaced_vaild_patterns: list[str] = re.findall(VALID_MACRO_PATTERN, generated_source_content)

    if len(unreplaced_vaild_patterns) == 0:
        return generated_source_content

    # 未置換のマクロが存在するのでエラー
    
    # 未置換のREPLACEマクロに対応するreplaceノードをxmlに追加
    for macro_str in unreplaced_vaild_patterns:
        replace_name = macro_str[1]  # 2番目のキャプチャグループ（YYYに相当）
        replace_name = replace_name.replace('"', '')
        replace_node = ET.Element("replace", attrib={"name": replace_name, "value":""})
        replace_macro_node.append(replace_node)

    invalid_patterns: list[str] = []
    for x in re.findall(VALID_MACRO_PATTERN, generated_source_content):
        if unreplaced_vaild_patterns.__contains__(x):
            continue
        else:
            invalid_patterns.append(x)
    
    for valid_pattern in unreplaced_vaild_patterns:
        pattern_str = f"CLN2D_GEN_REPLACE_{valid_pattern[0]}({valid_pattern[1]})"
        line_number = generated_source_content[:generated_source_content.find(pattern_str)].count('\n') + 1
        print_message(source_file, line_number, MessageKind.ERROR, f"Unreplaced macro found: {pattern_str}")

    for invalid_pattern in invalid_patterns:
        line_number = generated_source_content[:generated_source_content.find(invalid_pattern)].count('\n') + 1
        print_message(source_file, line_number, MessageKind.ERROR, f"Invalid macro found: {invalid_pattern}")


    return None
