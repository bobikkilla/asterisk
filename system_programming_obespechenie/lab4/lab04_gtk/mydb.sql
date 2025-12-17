-- Создание таблицы "Вид животного"
CREATE TABLE animal_types (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL
);

-- Вставка данных в таблицу "Вид животного"
INSERT INTO animal_types (name) VALUES 
('Собака'),
('Кошка'),
('Попугай'),
('Хомяк');

-- Создание таблицы "Животное"
CREATE TABLE animals (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    tag TEXT NOT NULL,           -- Кличка
    birth_date DATE NOT NULL,    -- Дата рождения
    gender TEXT NOT NULL,        -- Пол
    type_id INTEGER NOT NULL,    -- Ссылка на вид животного
    FOREIGN KEY (type_id)
       REFERENCES animal_types (id)
       ON UPDATE CASCADE
       ON DELETE CASCADE
);

-- Вставка данных в таблицу "Животное"
INSERT INTO animals (tag, birth_date, gender, type_id) VALUES
('Барсик', '2023-05-15', 'М', 2),   -- Кошка
('Шарик', '2022-11-20', 'М', 1),    -- Собака
('Гоша', '2024-01-10', 'М', 3),     -- Попугай
('Пушок', '2024-08-05', 'Ж', 4);    -- Хомяк

-- Создание таблицы "Корм"
CREATE TABLE feeds (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    type_id INTEGER NOT NULL,          -- Ссылка на вид животного
    manufacturer TEXT NOT NULL,        -- Изготовитель
    name TEXT NOT NULL,                -- Название корма
    price REAL NOT NULL,               -- Цена
    FOREIGN KEY (type_id)
       REFERENCES animal_types (id)
       ON UPDATE CASCADE
       ON DELETE CASCADE
);

-- Вставка данных в таблицу "Корм"
INSERT INTO feeds (type_id, manufacturer, name, price) VALUES
(1, 'Royal Canin', 'Adult Medium Dog', 1500.0),
(2, 'Purina', 'Cat Chow', 1200.0),
(3, 'Versele-Laga', 'Parrot Mix', 800.0),
(4, 'Vitakraft', 'Hamster Complete', 600.0);

-- Запрос: показать всех животных с их видами
SELECT
    animals.id,
    animals.tag AS кличка,
    animals.birth_date AS дата_рождения,
    animals.gender AS пол,
    animal_types.name AS вид_животного
FROM
    animals
INNER JOIN animal_types ON animals.type_id = animal_types.id;

-- Запрос: показать корма с указанием вида животного, для которого они предназначены
SELECT
    feeds.id,
    feeds.name AS название_корма,
    feeds.manufacturer AS изготовитель,
    feeds.price AS цена,
    animal_types.name AS вид_животного
FROM
    feeds
INNER JOIN animal_types ON feeds.type_id = animal_types.id;