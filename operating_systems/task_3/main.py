import sys
import math
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                            QLabel, QLineEdit, QPushButton, QMessageBox)
from PyQt5.QtCore import QThread, pyqtSignal

class PrimeCheckerThread(QThread):
    finished = pyqtSignal(str)

    def __init__(self, number):
        super().__init__()
        self.number = number

    def run(self):
        try:
            if self.number < 2:
                self.finished.emit(f"{self.number} не является простым числом")
                return

            for i in range(2, int(math.sqrt(self.number)) + 1):
                if self.number % i == 0:
                    self.finished.emit(f"{self.number} не является простым числом")
                    return

            self.finished.emit(f"{self.number} - простое число")
        except Exception as e:
            self.finished.emit(f"Ошибка: {str(e)}")

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setup_ui()
        self.thread = None

    def setup_ui(self):
        self.setWindowTitle("Проверка простого числа")
        self.setGeometry(100, 100, 300, 150)

        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)
        layout = QVBoxLayout()

        self.label = QLabel("Введите число:")
        self.input_box = QLineEdit()
        self.check_button = QPushButton("Проверить")
        self.check_button.clicked.connect(self.start_check)

        layout.addWidget(self.label)
        layout.addWidget(self.input_box)
        layout.addWidget(self.check_button)
        self.central_widget.setLayout(layout)

    def start_check(self):
        input_text = self.input_box.text()
        
        if not input_text:
            QMessageBox.warning(self, "Ошибка", "Введите число!")
            return

        try:
            number = int(input_text)
        except ValueError:
            QMessageBox.warning(self, "Ошибка", "Это не целое число!")
            return

        # Отключаем предыдущие подключения (если были)
        if self.thread:
            self.thread.finished.disconnect()
            self.thread.quit()
            self.thread.wait()

        self.check_button.setEnabled(False)
        
        self.thread = PrimeCheckerThread(number)
        self.thread.finished.connect(self.handle_result)
        self.thread.start()

    def handle_result(self, message):
        self.check_button.setEnabled(True)
        QMessageBox.information(self, "Результат", message)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())