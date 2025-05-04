import sys
import threading
import time
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                            QHBoxLayout, QLabel, QPushButton, QTextEdit,
                            QGroupBox)
from PyQt5.QtCore import Qt, pyqtSignal, QObject

class SharedResource:
    def __init__(self):
        self.data = []
        self.mutex = threading.Lock()
        self.counter = 1  # Счетчик для генерации новых данных

class WorkerSignals(QObject):
    update_log = pyqtSignal(str)
    update_data = pyqtSignal(str)

class WriterThread(threading.Thread):
    def __init__(self, resource, signals, writer_id):
        super().__init__()
        self.resource = resource
        self.signals = signals
        self.writer_id = writer_id
        self.running = True

    def run(self):
        while self.running:
            self.resource.mutex.acquire()
            try:
                # Генерируем новые данные
                data = f"W{self.writer_id}_data_{self.resource.counter}"
                self.resource.data.append(data)
                self.resource.counter += 1

                log_msg = f"Писатель {self.writer_id} записал: {data}"
                self.signals.update_log.emit(log_msg)
                self.signals.update_data.emit(f"Писатель {self.writer_id} последнее: {data}")
            finally:
                self.resource.mutex.release()
            time.sleep(3)  # Задержка для наглядности

    def stop(self):
        self.running = False

class ReaderThread(threading.Thread):
    def __init__(self, resource, signals, reader_id, writer_id):
        super().__init__()
        self.resource = resource
        self.signals = signals
        self.reader_id = reader_id
        self.writer_id = writer_id
        self.running = True
        self.read_data = []

    def run(self):
        while self.running:
            self.resource.mutex.acquire()
            try:
                # Фильтруем сообщения по идентификатору писателя
                filtered_data = [data for data in self.resource.data if f"W{self.writer_id}_" in data]
                if filtered_data:
                    data = filtered_data.pop(0)
                    self.resource.data.remove(data)
                    self.read_data.append(data)
                    log_msg = f"Читатель {self.reader_id} прочитал: {data}"
                    self.signals.update_log.emit(log_msg)
                    self.signals.update_data.emit(f"Читатель {self.reader_id} последнее: {data}")
            finally:
                self.resource.mutex.release()
            time.sleep(1)  # Задержка для наглядности

    def stop(self):
        self.running = False

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Использование мьютексов, синхронизация потоков")
        self.setGeometry(100, 100, 800, 600)

        self.shared_resource = SharedResource()
        self.threads = []
        self.signals = WorkerSignals()

        self.init_ui()
        self.signals.update_log.connect(self.update_log)
        self.signals.update_data.connect(self.update_data_display)

    def init_ui(self):
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        layout = QVBoxLayout()

        # Лог действий
        self.log_area = QTextEdit()
        self.log_area.setReadOnly(True)

        # Панель управления
        control_panel = QHBoxLayout()
        self.start_btn = QPushButton("Запустить потоки")
        self.start_btn.clicked.connect(self.start_threads)
        self.stop_btn = QPushButton("Остановить потоки")
        self.stop_btn.clicked.connect(self.stop_threads)
        control_panel.addWidget(self.start_btn)
        control_panel.addWidget(self.stop_btn)

        # Панели для отображения данных
        writers_group = QGroupBox("Писатели")
        writers_layout = QHBoxLayout()

        self.writer1_data = QTextEdit()
        self.writer1_data.setReadOnly(True)
        self.writer2_data = QTextEdit()
        self.writer2_data.setReadOnly(True)

        writers_layout.addWidget(QLabel("Писатель 1:"))
        writers_layout.addWidget(self.writer1_data)
        writers_layout.addWidget(QLabel("Писатель 2:"))
        writers_layout.addWidget(self.writer2_data)
        writers_group.setLayout(writers_layout)

        readers_group = QGroupBox("Читатели")
        readers_layout = QHBoxLayout()

        self.reader1_data = QTextEdit()
        self.reader1_data.setReadOnly(True)
        self.reader2_data = QTextEdit()
        self.reader2_data.setReadOnly(True)

        readers_layout.addWidget(QLabel("Читатель 1:"))
        readers_layout.addWidget(self.reader1_data)
        readers_layout.addWidget(QLabel("Читатель 2:"))
        readers_layout.addWidget(self.reader2_data)
        readers_group.setLayout(readers_layout)

        # Добавляем все на основной layout
        layout.addWidget(QLabel("Лог действий:"))
        layout.addWidget(self.log_area)
        layout.addLayout(control_panel)
        layout.addWidget(writers_group)
        layout.addWidget(readers_group)

        main_widget.setLayout(layout)

    def start_threads(self):
        self.stop_threads()  # Останавливаем старые потоки

        # Создаем и запускаем новые потоки
        writer1 = WriterThread(self.shared_resource, self.signals, 1)
        writer2 = WriterThread(self.shared_resource, self.signals, 2)
        reader1 = ReaderThread(self.shared_resource, self.signals, 1, 1)
        reader2 = ReaderThread(self.shared_resource, self.signals, 2, 2)

        for thread in [writer1, writer2, reader1, reader2]:
            thread.start()
            self.threads.append(thread)

        self.log_area.append("Все потоки запущены (бесконечный режим)")

    def stop_threads(self):
        for thread in self.threads:
            thread.stop()
            thread.join()
        self.threads = []
        self.log_area.append("Все потоки остановлены")

    def update_log(self, message):
        self.log_area.append(message)

    def update_data_display(self, message):
        if "Писатель 1" in message:
            self.writer1_data.setPlainText(message.split(": ")[1])
        elif "Писатель 2" in message:
            self.writer2_data.setPlainText(message.split(": ")[1])
        elif "Читатель 1" in message:
            self.reader1_data.setPlainText(message.split(": ")[1])
        elif "Читатель 2" in message:
            self.reader2_data.setPlainText(message.split(": ")[1])

    def closeEvent(self, event):
        self.stop_threads()
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
