import sys
import os
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QHBoxLayout, QLabel, QPushButton, QListWidget, QFileDialog)

class FileStructureApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Файловая структура диска")
        self.setGeometry(100, 100, 800, 600)

        self.current_path = os.path.abspath(os.sep)  # Корневой каталог
        self.history = []

        self.init_ui()
        self.update_directory_list()

    def init_ui(self):
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        layout = QVBoxLayout()

        # Метка для отображения текущего пути
        self.path_label = QLabel(f"Текущий путь: {self.current_path}")
        layout.addWidget(self.path_label)

        # Список для отображения содержимого каталога
        self.directory_list = QListWidget()
        layout.addWidget(self.directory_list)

        # Панель управления
        control_panel = QHBoxLayout()
        self.back_btn = QPushButton("Назад")
        self.back_btn.clicked.connect(self.go_back)
        self.display_btn = QPushButton("Отобразить")
        self.display_btn.clicked.connect(self.display_selected_directory)
        control_panel.addWidget(self.back_btn)
        control_panel.addWidget(self.display_btn)
        layout.addLayout(control_panel)

        main_widget.setLayout(layout)

    def update_directory_list(self):
        self.directory_list.clear()
        try:
            items = os.listdir(self.current_path)
            for item in items:
                self.directory_list.addItem(item)
        except PermissionError:
            self.directory_list.addItem("Доступ запрещен")

    def display_selected_directory(self):
        selected_item = self.directory_list.currentItem()
        if selected_item:
            new_path = os.path.join(self.current_path, selected_item.text())
            if os.path.isdir(new_path):
                self.history.append(self.current_path)
                self.current_path = new_path
                self.path_label.setText(f"Текущий путь: {self.current_path}")
                self.update_directory_list()

    def go_back(self):
        if self.history:
            self.current_path = self.history.pop()
            self.path_label.setText(f"Текущий путь: {self.current_path}")
            self.update_directory_list()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = FileStructureApp()
    window.show()
    sys.exit(app.exec_())
