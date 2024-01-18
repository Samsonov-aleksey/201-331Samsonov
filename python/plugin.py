import cutter
import base64
import re

from PySide2.QtWidgets import QAction, QLabel, QPushButton, QVBoxLayout, QWidget, QFileDialog

class MyDockWidget(cutter.CutterDockWidget):
    def __init__(self, parent, action):
        super(MyDockWidget, self).__init__(parent, action)
        self.setObjectName("Search")
        self.setWindowTitle("Searcher")

        self._label = QLabel(self)
        self.setWidget(self._label)

        self.encoding_button = QPushButton("Find Strings", self)

        self.layout = QVBoxLayout()
        self.layout.addWidget(self.encoding_button)
        self.setLayout(self.layout)

        self.encoding_button.clicked.connect(self.find_encoding_strings)

    def find_encoding_strings(self):
        file_path = QFileDialog.getOpenFileName(caption='Select File')[0]
        with open(file_path, 'rb') as file:
            binary_data = file.read()

        base64_strings = []
        hex_strings = []
        pattern = re.compile(b'(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?')
        hex_pattern = re.compile(b'([0-9A-Fa-f]{2})+')

        for match in pattern.finditer(binary_data):
            try:
                decoded_base_word = base64.b64decode(match.group()).decode('utf-8')
                if decoded_base_word:
                    base64_strings.append(decoded_base_word)
            except Exception as e:
                pass

        for match in hex_pattern.finditer(binary_data):
            try:
                decoded_hex_word = bytes.fromhex(match.group().decode('utf-8')).decode('utf-8')
                if decoded_hex_word:
                    hex_strings.append(decoded_hex_word)
            except Exception as e:
                pass

        self._label.setText('Base64 Strings:\n' + '\n'.join(base64_strings[:0]) + '\n\nHex Strings:\n' + '\n'.join(hex_strings[:40]))

class MyCutterPlugin(cutter.CutterPlugin):
    name = "My Plugin"
    description = "This plugin does awesome things!"
    version = "1.0"
    author = "me"

    def setupPlugin(self):
        pass

    def setupInterface(self, main):
        action = QAction("My Plugin", main)
        action.setCheckable(True)
        widget = MyDockWidget(main, action)
        main.addPluginDockWidget(widget, action)

def create_cutter_plugin():
    return MyCutterPlugin()
