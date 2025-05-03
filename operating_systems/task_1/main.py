import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QLabel, QPushButton, 
                             QFileDialog, QVBoxLayout, QWidget, QHBoxLayout)
from PyQt5.QtGui import QPixmap, QKeyEvent
from PyQt5.QtCore import Qt, QSize, QEvent

class ImageViewer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Просмотреть изображение")
        self.setGeometry(100, 100, 800, 600)
        
        # Главный виджет и layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # Основной вертикальный layout (для PictureBox + кнопок)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setContentsMargins(5, 5, 5, 5)
        
        # PictureBox (QLabel с изображением)
        self.image_label = QLabel()
        self.image_label.setAlignment(Qt.AlignCenter)
        self.image_label.setStyleSheet("background-color: gray;")
        self.image_label.setScaledContents(False)
        
        # Горизонтальный layout для кнопок
        button_layout = QHBoxLayout()
        
        self.open_button = QPushButton("Открыть изображение")
        self.open_button.clicked.connect(self.open_image)
        
        # self.zoom_in_button = QPushButton("+ Увеличить (Ctrl+)")
        # self.zoom_in_button.clicked.connect(self.zoom_in)
        
        # self.zoom_out_button = QPushButton("- Уменьшить (Ctrl-)")
        # self.zoom_out_button.clicked.connect(self.zoom_out)
        
        button_layout.addStretch()
        button_layout.addWidget(self.open_button)
        # button_layout.addWidget(self.zoom_out_button)
        # button_layout.addWidget(self.zoom_in_button)
        
        main_layout.addWidget(self.image_label, stretch=1)
        main_layout.addLayout(button_layout)
        
        # Сохраняем оригинальное изображение и масштаб
        self.original_pixmap = None
        self.current_scale = 1.0
        
        # Минимальный и максимальный масштаб (чтобы не было слишком маленьким/большим)
        self.min_scale = 0.1
        self.max_scale = 5.0
    
    def open_image(self):
        """Открывает изображение и подгоняет окно под его размер"""
        file_filter = "Image Files (*.bmp *.jpg *.jpeg *.gif *.png);;All Files (*)"
        file_name, _ = QFileDialog.getOpenFileName(self, "Open Image", "", file_filter)
        
        if file_name:
            try:
                self.original_pixmap = QPixmap(file_name)
                if self.original_pixmap.isNull():
                    raise ValueError("Не удалось загрузить изображение")
                
                self.current_scale = 1.0
                self.resize_to_image()
                self.resize_image()
                
            except Exception as e:
                self.image_label.setText(f"Ошибка: {str(e)}")
    
    def resize_to_image(self):
        """Подгоняет размер окна под изображение"""
        if not self.original_pixmap:
            return
        
        screen_size = QApplication.primaryScreen().availableSize()
        max_width = int(screen_size.width() * 0.8)
        max_height = int(screen_size.height() * 0.8)
        
        img_width = self.original_pixmap.width()
        img_height = self.original_pixmap.height()
        
        if img_width > max_width or img_height > max_height:
            scaled_pixmap = self.original_pixmap.scaled(
                QSize(max_width, max_height),
                Qt.KeepAspectRatio,
                Qt.SmoothTransformation
            )
            self.resize(scaled_pixmap.width(), scaled_pixmap.height() + 50)
        else:
            self.resize(img_width, img_height + 50)
    
    def resize_image(self):
        """Масштабирует изображение с учетом current_scale"""
        if not self.original_pixmap:
            return
        
        new_width = int(self.original_pixmap.width() * self.current_scale)
        new_height = int(self.original_pixmap.height() * self.current_scale)
        
        scaled_pixmap = self.original_pixmap.scaled(
            new_width, new_height,
            Qt.KeepAspectRatio,
            Qt.SmoothTransformation
        )
        self.image_label.setPixmap(scaled_pixmap)
    
    def zoom_in(self):
        """Увеличивает масштаб на 10%"""
        if self.original_pixmap and self.current_scale < self.max_scale:
            self.current_scale *= 1.1
            self.resize_image()
    
    def zoom_out(self):
        """Уменьшает масштаб на 10%"""
        if self.original_pixmap and self.current_scale > self.min_scale:
            self.current_scale *= 0.9
            self.resize_image()
    
    def keyPressEvent(self, event: QKeyEvent):
        """Обработка Ctrl+ и Ctrl- для масштабирования"""
        if event.modifiers() == Qt.ControlModifier:
            if event.key() == Qt.Key_Plus or event.key() == Qt.Key_Equal:  # "+" или "="
                self.zoom_in()
            elif event.key() == Qt.Key_Minus:  # "-"
                self.zoom_out()
        super().keyPressEvent(event)
    
    def wheelEvent(self, event):
        """Масштабирование колесиком мыши (при зажатом Ctrl)"""
        if event.modifiers() == Qt.ControlModifier:
            delta = event.angleDelta().y()
            if delta > 0:  # Прокрутка вверх
                self.zoom_in()
            elif delta < 0:  # Прокрутка вниз
                self.zoom_out()
        else:
            super().wheelEvent(event)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    
    window = ImageViewer()
    window.show()
    
    sys.exit(app.exec_())