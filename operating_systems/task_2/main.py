import os
from PyQt5.QtWidgets import (QApplication, QMainWindow, QListWidget, 
                             QPushButton, QVBoxLayout, QWidget, QHBoxLayout,
                             QMenu)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import QFileDialog

class FileListApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Работа с файлами")
        self.setGeometry(100, 100, 500, 400)
        
        # Главный виджет и layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout(central_widget)
        
        # ListWidget для файлов
        self.list_widget = QListWidget()
        layout.addWidget(self.list_widget)
        
        # Фрейм для кнопок
        button_frame = QWidget()
        button_layout = QHBoxLayout(button_frame)
        
        # Кнопка сортировки с выпадающим меню
        self.sort_button = QPushButton("Сортировка")
        self.sort_menu = QMenu()
        
        # Добавляем действия в меню сортировки
        self.sort_menu.addAction("По размеру (возр.)", lambda: self.sort_files('size_asc'))
        self.sort_menu.addAction("По размеру (убыв.)", lambda: self.sort_files('size_desc'))
        self.sort_menu.addAction("По имени (A-Z)", lambda: self.sort_files('name_asc'))
        self.sort_menu.addAction("По имени (Z-A)", lambda: self.sort_files('name_desc'))
        self.sort_menu.addAction("По расширению", lambda: self.sort_files('ext'))
        
        self.sort_button.setMenu(self.sort_menu)
        button_layout.addWidget(self.sort_button)
        
        # Кнопка добавления файлов
        self.add_button = QPushButton("Добавить файлы")
        self.add_button.clicked.connect(self.add_files)
        button_layout.addWidget(self.add_button)
        
        layout.addWidget(button_frame)
        self.files = {}
    
    def add_files(self):
        """Добавляет файлы в список"""
        file_paths, _ = QFileDialog.getOpenFileNames(
            self, "Выберите файлы", "", "Все файлы (*)"
        )
        for path in file_paths:
            if path not in self.files:
                size = os.path.getsize(path)
                filename = os.path.basename(path)
                ext = os.path.splitext(path)[1].lower()
                self.files[path] = {
                    'size': size,
                    'name': filename,
                    'ext': ext
                }
                self.list_widget.addItem(f"{filename} ({size} bytes)")
    
    def sort_files(self, mode):
        """Сортирует файлы по выбранному критерию"""
        if not self.files:
            return
        
        # Определяем ключ и направление сортировки
        if mode == 'size_asc':
            key = lambda x: x[1]['size']
            reverse = False
        elif mode == 'size_desc':
            key = lambda x: x[1]['size']
            reverse = True
        elif mode == 'name_asc':
            key = lambda x: x[1]['name'].lower()
            reverse = False
        elif mode == 'name_desc':
            key = lambda x: x[1]['name'].lower()
            reverse = True
        elif mode == 'ext':
            key = lambda x: x[1]['ext']
            reverse = False
        
        sorted_files = sorted(self.files.items(), key=key, reverse=reverse)
        self.list_widget.clear()
        
        for path, data in sorted_files:
            self.list_widget.addItem(f"{data['name']} ({data['size']} bytes)")

if __name__ == "__main__":
    app = QApplication([])
    window = FileListApp()
    window.show()
    app.exec_()