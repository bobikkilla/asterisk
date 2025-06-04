import tkinter as tk
from tkinter import messagebox
from itertools import product

class ChessBoard:
    def __init__(self, root):
        self.root = root
        self.root.title("Checkmate&Stalemate Finder")
        self.canvas = tk.Canvas(root, width=400, height=400)
        self.canvas.pack()

        self.draw_chessboard()

        # Bind click event to place kings
        self.canvas.bind("<Button-1>", self.place_king)

        # Information box
        self.info_label = tk.Label(root, text="Place the Black and White Kings")
        self.info_label.pack()

        # Navigation buttons
        self.next_button = tk.Button(root, text="Next Position", command=self.next_position)
        self.next_button.pack(side=tk.LEFT)

        self.filter_button = tk.Button(root, text="Filter by Pieces", command=self.filter_by_pieces)
        self.filter_button.pack(side=tk.RIGHT)

        # Store positions
        self.black_king_position = None
        self.white_king_position = None
        self.all_positions = []
        self.positions = []
        self.current_position_index = 0

    def draw_chessboard(self):
        for row in range(8):
            for col in range(8):
                x1 = col * 50
                y1 = row * 50
                x2 = x1 + 50
                y2 = y1 + 50
                color = "light goldenrod" if (row + col) % 2 == 0 else "saddle brown"
                self.canvas.create_rectangle(x1, y1, x2, y2, fill=color)

    def place_king(self, event):
        col = event.x // 50
        row = event.y // 50
        position = (row, col)

        if not self.black_king_position:
            self.black_king_position = position
            self.canvas.create_text(col * 50 + 25, row * 50 + 25, text="♚", font=("Arial", 30), tags="king")
        elif not self.white_king_position:
            self.white_king_position = position
            self.canvas.create_text(col * 50 + 25, row * 50 + 25, text="♔", font=("Arial", 30), tags="king")
            self.calculate_positions()

    def get_adjacent_squares(self, pos):
        r, c = pos
        squares = []
        for dr in [-1, 0, 1]:
            for dc in [-1, 0, 1]:
                if dr == 0 and dc == 0:
                    continue
                nr = r + dr
                nc = c + dc
                if 0 <= nr < 8 and 0 <= nc < 8:
                    squares.append((nr, nc))
        return squares

    def is_king_attack(self, target, attacker):
        r1, c1 = target
        r2, c2 = attacker
        return abs(r1 - r2) <= 1 and abs(c1 - c2) <= 1

    def is_knight_attack(self, target, pos):
        r, c = pos
        tr, tc = target
        dr = abs(r - tr)
        dc = abs(c - tc)
        return (dr == 2 and dc == 1) or (dr == 1 and dc == 2)

    def is_path_clear(self, start, end, blocked_positions):
        if start[0] == end[0]:
            row = start[0]
            min_col = min(start[1], end[1])
            max_col = max(start[1], end[1])
            for c in range(min_col + 1, max_col):
                if (row, c) in blocked_positions:
                    return False
            return True
        elif start[1] == end[1]:
            col = start[1]
            min_row = min(start[0], end[0])
            max_row = max(start[0], end[0])
            for r in range(min_row + 1, max_row):
                if (r, col) in blocked_positions:
                    return False
            return True
        elif abs(start[0] - end[0]) == abs(start[1] - end[1]):
            row_step = 1 if end[0] > start[0] else -1
            col_step = 1 if end[1] > start[1] else -1
            steps = abs(end[0] - start[0]) - 1
            r, c = start[0] + row_step, start[1] + col_step
            for _ in range(steps):
                if (r, c) in blocked_positions:
                    return False
                r += row_step
                c += col_step
            return True
        else:
            return False

    def is_sliding_attack(self, target_pos, attacker_pos, attacker_type, other_blockers):
        if attacker_type in ['Rook', 'Queen']:
            if attacker_pos[0] == target_pos[0] or attacker_pos[1] == target_pos[1]:
                if self.is_path_clear(attacker_pos, target_pos, other_blockers):
                    return True
        if attacker_type in ['Bishop', 'Queen']:
            if abs(attacker_pos[0] - target_pos[0]) == abs(attacker_pos[1] - target_pos[1]):
                if self.is_path_clear(attacker_pos, target_pos, other_blockers):
                    return True
        return False

    def is_black_king_attacked(self, bk_pos, wk_pos, p1_pos, p1_type, p2_pos, p2_type):
        if self.is_king_attack(bk_pos, wk_pos):
            return True
        if p1_type == 'Knight':
            if self.is_knight_attack(bk_pos, p1_pos):
                return True
        else:
            if self.is_sliding_attack(bk_pos, p1_pos, p1_type, [wk_pos, p2_pos]):
                return True
        if p2_type == 'Knight':
            if self.is_knight_attack(bk_pos, p2_pos):
                return True
        else:
            if self.is_sliding_attack(bk_pos, p2_pos, p2_type, [wk_pos, p1_pos]):
                return True
        return False

    def is_square_attacked(self, square, wk_pos, p1_pos, p1_type, p2_pos, p2_type):
        if self.is_king_attack(square, wk_pos):
            return True
        if p1_type == 'Knight':
            if self.is_knight_attack(square, p1_pos):
                return True
        else:
            if self.is_sliding_attack(square, p1_pos, p1_type, [wk_pos, p2_pos]):
                return True
        if p2_type == 'Knight':
            if self.is_knight_attack(square, p2_pos):
                return True
        else:
            if self.is_sliding_attack(square, p2_pos, p2_type, [wk_pos, p1_pos]):
                return True
        return False

    def is_square_safe(self, square, occupied, wk_pos, p1_pos, p1_type, p2_pos, p2_type):
        return square not in occupied and not self.is_square_attacked(square, wk_pos, p1_pos, p1_type, p2_pos, p2_type)

    def evaluate_position(self, bk_pos, wk_pos, p1_pos, p1_type, p2_pos, p2_type):
        attacked = self.is_black_king_attacked(bk_pos, wk_pos, p1_pos, p1_type, p2_pos, p2_type)
        occupied = {wk_pos, p1_pos, p2_pos}
        available_moves = 0
        for adj_square in self.get_adjacent_squares(bk_pos):
            if self.is_square_safe(adj_square, occupied, wk_pos, p1_pos, p1_type, p2_pos, p2_type):
                available_moves += 1
        if attacked:
            return 'checkmate' if available_moves == 0 else 'other'
        else:
            return 'stalemate' if available_moves == 0 else 'other'

    def calculate_positions(self):
        bk_pos = self.black_king_position
        wk_pos = self.white_king_position
        if not bk_pos or not wk_pos:
            return

        occupied_kings = {bk_pos, wk_pos}
        available_squares = [(r, c) for r in range(8) for c in range(8) if (r, c) not in occupied_kings]
        piece_types = ['Queen', 'Rook', 'Bishop', 'Knight']
        self.all_positions.clear()

        for p1_type, p2_type in product(piece_types, repeat=2):
            for s1 in available_squares:
                for s2 in available_squares:
                    if s1 == s2:
                        continue
                    status = self.evaluate_position(bk_pos, wk_pos, s1, p1_type, s2, p2_type)
                    if status == 'checkmate':
                        self.all_positions.append({
                            'type': 'checkmate',
                            'pieces': [p1_type, p2_type],
                            'piece_positions': [s1, s2],
                            'description': f"Checkmate with {p1_type} and {p2_type}"
                        })
                    elif status == 'stalemate':
                        self.all_positions.append({
                            'type': 'stalemate',
                            'pieces': [p1_type, p2_type],
                            'description': f"Stalemate with {p1_type} and {p2_type}"
                        })

        self.positions = self.all_positions.copy()
        self.current_position_index = 0
        self.show_position()

    def show_position(self):
        if not self.positions:
            self.info_label.config(text="No positions found.")
            return

        self.canvas.delete("additional")

        current = self.positions[self.current_position_index]
        piece_types = current['pieces']
        piece_positions = current.get('piece_positions', [(0, 0), (0, 0)])

        for i in range(2):
            pos = piece_positions[i]
            symbol = self.get_piece_symbol(piece_types[i])
            self.draw_additional_piece(pos, symbol)

        checkmates = [p for p in self.positions if p['type'] == 'checkmate']
        stalemates = [p for p in self.positions if p['type'] == 'stalemate']
        current_desc = current['description']
        total = len(self.positions)
        idx = self.current_position_index + 1
        self.info_label.config(text=f"Checkmates: {len(checkmates)}, Stalemates: {len(stalemates)}. {current_desc} ({idx}/{total})")

    def draw_additional_piece(self, pos, symbol):
        r, c = pos
        x = c * 50 + 25
        y = r * 50 + 25
        self.canvas.create_text(x, y, text=symbol, font=("Pecita", 30), tags="additional")

    def get_piece_symbol(self, piece_type):
        symbols = {
            'Queen': '♕',
            'Rook': '♖',
            'Bishop': '♗',
            'Knight': '♘'
        }
        return symbols.get(piece_type, '?')

    def next_position(self):
        if self.positions:
            self.current_position_index = (self.current_position_index + 1) % len(self.positions)
            self.show_position()

    def filter_by_pieces(self):
        dialog = tk.Toplevel(self.root)
        dialog.title("Filter by Pieces")

        tk.Label(dialog, text="Select two pieces:").pack()
        pieces = ['Queen', 'Rook', 'Bishop', 'Knight']
        var1 = tk.StringVar(value=pieces[0])
        var2 = tk.StringVar(value=pieces[0])
        tk.OptionMenu(dialog, var1, *pieces).pack()
        tk.OptionMenu(dialog, var2, *pieces).pack()

        def apply_filter():
            p1, p2 = var1.get(), var2.get()
            self.positions = [pos for pos in self.all_positions if
                              (pos['pieces'][0] == p1 and pos['pieces'][1] == p2) or
                              (pos['pieces'][0] == p2 and pos['pieces'][1] == p1)]
            self.current_position_index = 0
            self.show_position()
            dialog.destroy()

        tk.Button(dialog, text="Apply", command=apply_filter).pack()

if __name__ == "__main__":
    root = tk.Tk()
    app = ChessBoard(root)
    root.mainloop()