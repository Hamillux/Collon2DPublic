import sys
import xml.etree.ElementTree as ET
from enum import Enum

class MessageKind(Enum):
    NOTE = 0
    WARNING = 1
    ERROR = 2

message_kind_str = {
    MessageKind.NOTE: "note",
    MessageKind.WARNING: "warning",
    MessageKind.ERROR: "error"
}

# メッセージを出力
def print_message(filename: str, line: int, kind: MessageKind, description):
    if kind == MessageKind.NOTE:
        sys.stdout.write(f"{filename}({line}): {message_kind_str[MessageKind.NOTE]}: {description}\n")
        return
    
    sys.stderr.write(f"{filename}({line}): {message_kind_str[kind]}: {description}\n")

# 親ノードから, 指定された子ノードを削除する
def remove_children_nodes(parent_node: ET.Element, remove_children_list: list[ET.Element]):
    if remove_children_list.__len__() == 0:
        return

    print('remove_children_nodes called')
    for child in remove_children_list:
        parent_node.remove(child)