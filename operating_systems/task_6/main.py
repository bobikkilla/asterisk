import sys
import random
import threading
import time
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QHBoxLayout, QLabel, QPushButton, QMessageBox, QFrame, QTextEdit)
from PyQt5.QtCore import pyqtSignal, QObject, Qt, QTimer, QMutex
from PyQt5.QtGui import QFont

class WorkerSignals(QObject):
    update_number = pyqtSignal(int, int)

class NumberGeneratorThread(threading.Thread):
    def __init__(self, signals, index, mutex):
        super().__init__()
        self.signals = signals
        self.index = index
        self.mutex = mutex
        self.running = True
        self.current_number = 0

    def run(self):
        while self.running:
            self.current_number = random.randint(0, 9)
            self.mutex.lock()
            self.signals.update_number.emit(self.index, self.current_number)
            self.mutex.unlock()
            time.sleep(0.1)

    def stop(self):
        self.running = False

class SlotMachineApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Однорукий бандит")
        self.setGeometry(100, 100, 600, 500)

        self.balance = 100  # Начальный баланс
        self.bet = 10  # Ставка
        self.threads = []
        self.signals = WorkerSignals()
        self.numbers = [7, 7, 7]  # Текущие числа на барабанах
        self.spinning = False
        self.mutex = QMutex()

        self.init_ui()
        self.signals.update_number.connect(self.update_number)

    def init_ui(self):
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        layout = QVBoxLayout()

        # Верхняя панель с балансом
        top_panel = QHBoxLayout()
        self.balance_label = QLabel(f"Баланс: {self.balance} spasibo")
        self.balance_label.setFont(QFont("Arial", 16))
        top_panel.addWidget(self.balance_label)
        layout.addLayout(top_panel)

        # Барабаны
        self.reels_frame = QFrame()
        self.reels_frame.setFrameStyle(QFrame.Box | QFrame.Raised)
        reels_layout = QHBoxLayout()
        self.number_labels = [QLabel(str(num)) for num in self.numbers]
        for label in self.number_labels:
            label.setFont(QFont("Arial", 48))
            label.setAlignment(Qt.AlignCenter)
            label.setFrameStyle(QFrame.Box | QFrame.Raised)
            reels_layout.addWidget(label)
        self.reels_frame.setLayout(reels_layout)
        layout.addWidget(self.reels_frame)

        # Таблица выигрышных комбинаций
        self.combinations_label = QLabel("Выигрышные комбинации:")
        self.combinations_label.setFont(QFont("Arial", 14))
        layout.addWidget(self.combinations_label)

        self.combinations_text = QTextEdit()
        self.combinations_text.setReadOnly(True)
        self.combinations_text.setFont(QFont("Arial", 12))
        combinations = (
            "777: x100\n"
            "555: x50\n"
            "333: x30\n"
            "111: x20\n"
            "000: x10\n"
            "123: x5\n"
            "0** (0 на первой позиции): x1.5\n"
            "77* или *77 (две семерки на первых двух или последних двух позициях): x5\n"
            "7*7 (семерки на первой и третьей позициях): x4\n"
            "7**, *7*, **7 (семерка на любой позиции): x2\n"
            "*00* (ноль на второй и третьей позициях): x3"
        )
        self.combinations_text.setText(combinations)
        layout.addWidget(self.combinations_text)

        # Панель управления
        control_panel = QHBoxLayout()
        self.start_btn = QPushButton("Старт")
        self.start_btn.setFont(QFont("Arial", 16))
        self.start_btn.clicked.connect(self.start_spin)
        self.add_balance_btn = QPushButton("Пополнить баланс")
        self.add_balance_btn.setFont(QFont("Arial", 16))
        self.add_balance_btn.clicked.connect(self.confirm_add_balance)
        control_panel.addWidget(self.start_btn)
        control_panel.addWidget(self.add_balance_btn)
        layout.addLayout(control_panel)

        main_widget.setLayout(layout)

    def start_spin(self):
        if self.balance < self.bet:
            QMessageBox.warning(self, "Недостаточно средств", "Пополните баланс для продолжения игры.")
            return

        if self.spinning:
            return

        self.balance -= self.bet
        self.balance_label.setText(f"Баланс: {self.balance} spasibo")

        self.spinning = True
        self.start_btn.setEnabled(False)

        # Создаем и запускаем новые потоки
        for i in range(3):
            thread = NumberGeneratorThread(self.signals, i, self.mutex)
            thread.start()
            self.threads.append(thread)

        # Таймер для остановки вращения через 3 секунды
        QTimer.singleShot(200, self.stop_spin)

    def stop_spin(self):
        # Останавливаем все потоки
        for thread in self.threads:
            thread.stop()
            thread.join()
        
        # Фиксируем финальные значения
        self.mutex.lock()
        for i, thread in enumerate(self.threads):
            self.numbers[i] = thread.current_number
            #self.numbers = [0,0,7]
            self.number_labels[i].setText(str(thread.current_number))
        self.mutex.unlock()
        
        self.threads = []
        self.spinning = False
        self.start_btn.setEnabled(True)

        # Анализируем результат после остановки
        self.analyze_result()

    def update_number(self, index, number):
        self.mutex.lock()
        self.number_labels[index].setText(str(number))
        self.mutex.unlock()

    def analyze_result(self):
        num1, num2, num3 = self.numbers
        reward = 0

        # Проверяем комбинации в порядке убывания приоритета
        if num1 == num2 == num3 == 7:
            reward = 100  # 777
        elif num1 == num2 == num3 == 5:
            reward = 50  # 555
        elif num1 == num2 == num3 == 3:
            reward = 30  # 333
        elif num1 == num2 == num3 == 1:
            reward = 20  # 111
        elif num1 == num2 == num3 == 0:
            reward = 10  # 000
        elif num1 == 1 and num2 == 2 and num3 == 3:
            reward = 5  # 123
        elif ((num1 == 7 and num2 == 7) or (num2 == 7 and num3 == 7)) and (reward < 5):
            reward = 5  # 77* или *77
        elif (num1 == 7 and num3 == 7) and (reward < 4):
            reward = 4  # 7*7
        elif ((num1 == 0 and num2 == 0) or (num2 == 0 and num3 == 0)) and (reward < 3):
            reward = 3  # *00*
        elif (num1 == 7 or num2 == 7 or num3 == 7) and (reward < 2):
            reward = 2  # 7**, *7*, **7        
        elif num1 == 0 and num2 != 0 and (reward < 1.5):
            reward = 1.5 # 0**

        if reward > 1:
            self.balance += self.bet * reward
            self.balance_label.setText(f"Баланс: {self.balance} spasibo")
            QMessageBox.information(self, "Выигрыш!", f"Вы выиграли {(self.bet * reward)} spasibo!")

    def confirm_add_balance(self):
        reply = QMessageBox.question(self, 'Пополнение баланса',
                                     "Вы уверены, что хотите пополнить баланс на 200 spasibo?",
                                     QMessageBox.Yes | QMessageBox.No, QMessageBox.No)

        if reply == QMessageBox.Yes:
            self.add_balance()

    def add_balance(self):
        self.balance += 200
        self.balance_label.setText(f"Баланс: {self.balance} spasibo")
        QMessageBox.information(self, "Баланс пополнен", "Ваш баланс пополнен на 200 spasibo")

    def closeEvent(self, event):
        self.stop_spin()
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SlotMachineApp()
    window.show()
    sys.exit(app.exec_())