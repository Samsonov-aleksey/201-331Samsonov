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
            text = file.read()
        # Поиск строк в кодировке base64
        base64_strings = []
        hex_strings = []
        pattern = re.compile("[a-zA-Z][0-9a-zA-Z]*$")

        for word in text.split():
            try:
                decoded_base_word = base64.b64decode(word).decode('utf-8')
                if decoded_base_word and re.match(pattern, decoded_base_word) and len(decoded_base_word) >= 1:
                    base64_strings.append(decoded_base_word)
            except:
                pass

            try:
                # Check for hex encoded strings
                decoded_hex_word = bytes.fromhex(word.decode('utf-8')).decode('utf-8')
                if decoded_hex_word and re.match(pattern, decoded_hex_word) and len(decoded_hex_word) >= 1:
                    hex_strings.append(decoded_hex_word)
            except:
                pass

        self._label.setText('Base64 Strings:\n' + '\n'.join(base64_strings[:30]) + '\n\nHex Strings:\n' + '\n'.join(hex_strings[:30]))


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
