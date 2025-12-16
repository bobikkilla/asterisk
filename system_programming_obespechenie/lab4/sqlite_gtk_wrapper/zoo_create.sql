-- zoo_create.sql
CREATE TABLE IF NOT EXISTS Вид_животного (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    Название TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS Животное (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    Кличка TEXT NOT NULL,
    Дата_рождения DATE,
    Пол TEXT CHECK(Пол IN ('М', 'Ж')),
    вид_id INTEGER NOT NULL,
    FOREIGN KEY (вид_id) REFERENCES Вид_животного(id) ON DELETE RESTRICT
);

CREATE TABLE IF NOT EXISTS Корм (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    Вид TEXT NOT NULL,
    Изготовитель TEXT,
    Название TEXT NOT NULL,
    Цена REAL CHECK(Цена > 0)
);

CREATE TABLE IF NOT EXISTS Животное_Корм (
    животное_id INTEGER NOT NULL,
    корм_id INTEGER NOT NULL,
    PRIMARY KEY (животное_id, корм_id),
    FOREIGN KEY (животное_id) REFERENCES Животное(id) ON DELETE CASCADE,
    FOREIGN KEY (корм_id) REFERENCES Корм(id) ON DELETE CASCADE
);

-- Пример данных
INSERT OR IGNORE INTO Вид_животного (Название) VALUES
    ('Собака'), ('Кошка'), ('Хомяк'), ('Попугай');

INSERT OR IGNORE INTO Корм (Вид, Изготовитель, Название, Цена) VALUES
    ('сухой', 'Royal Canin', 'Adult Dog', 1200.0),
    ('влажный', 'Whiskas', 'Кошачий обед', 85.5),
    ('гранулы', 'Vitakraft', 'Хомячье счастье', 320.0);

-- Пример животного
INSERT OR IGNORE INTO Животное (Кличка, Дата_рождения, Пол, вид_id)
    SELECT 'Шарик', '2022-03-15', 'М', id FROM Вид_животного WHERE Название = 'Собака';

-- Связь Шарик → Royal Canin
INSERT OR IGNORE INTO Животное_Корм (животное_id, корм_id)
    SELECT ж.id, к.id
    FROM Животное ж
    JOIN Вид_животного в ON ж.вид_id = в.id
    JOIN Корм к ON к.Название = 'Adult Dog'
    WHERE ж.Кличка = 'Шарик' AND в.Название = 'Собака';