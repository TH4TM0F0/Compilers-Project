import subprocess
import tkinter as tk
from tkinter.scrolledtext import ScrolledText
from tkinter import filedialog, messagebox

EXE = "./exe/compiler.exe"
DEFAULT_FILE = "Text Files/input.txt"   # change if you want


class LineNumberedText(tk.Frame):
    def __init__(self, master, **kwargs):
        super().__init__(master)

        # Left gutter for line numbers
        self.linenumbers = tk.Text(
            self,
            width=5,
            padx=4,
            takefocus=0,
            border=0,
            background="#f0f0f0",
            state="disabled",
            wrap="none"
        )
        self.linenumbers.pack(side="left", fill="y")

        # Main editor
        self.text = ScrolledText(self, wrap="none", undo=True, **kwargs)
        self.text.pack(side="right", fill="both", expand=True)

        # Hook events to update line numbers
        self.text.bind("<KeyRelease>", self.update_linenumbers)
        self.text.bind("<MouseWheel>", self.update_linenumbers)     # Windows scroll
        self.text.bind("<ButtonRelease-1>", self.update_linenumbers)

        # For Linux/WSL scrolling
        self.text.bind("<Button-4>", self.update_linenumbers)
        self.text.bind("<Button-5>", self.update_linenumbers)

        # Also update periodically (keeps it stable)
        self.after(100, self.update_linenumbers)

    def update_linenumbers(self, event=None):
        self.linenumbers.config(state="normal")
        self.linenumbers.delete("1.0", "end")

        # visible region
        start_index = self.text.index("@0,0")
        start_line = int(start_index.split(".")[0])

        # Estimate how many lines are visible
        height = self.text.winfo_height()
        line_height = int(self.text.dlineinfo("1.0")[3]) if self.text.dlineinfo("1.0") else 15
        visible_lines = max(1, height // line_height)

        for i in range(start_line, start_line + visible_lines + 2):
            self.linenumbers.insert("end", f"{i}\n")

        self.linenumbers.config(state="disabled")


def run_compiler():
    code = editor.text.get("1.0", "end-1c")

    # Option A: stdin (only if compiler supports stdin)
    # p = subprocess.run([EXE], input=code, text=True, capture_output=True)

    # Option B: write to file then run with file path (works with your current compiler main)
    temp_file = DEFAULT_FILE
    try:
        with open(temp_file, "w", encoding="utf-8") as f:
            f.write(code)
    except Exception as e:
        messagebox.showerror("File write error", str(e))
        return

    p = subprocess.run([EXE, temp_file], text=True, capture_output=True)

    output.delete("1.0", "end")
    output.insert("1.0", p.stdout)
    if p.stderr:
        output.insert("end", "\n--- STDERR ---\n")
        output.insert("end", p.stderr)


def load_file():
    path = filedialog.askopenfilename(
        initialdir="Text Files",
        title="Open file",
        filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
    )
    if not path:
        return
    try:
        with open(path, "r", encoding="utf-8") as f:
            content = f.read()
        editor.text.delete("1.0", "end")
        editor.text.insert("1.0", content)
    except Exception as e:
        messagebox.showerror("Open error", str(e))


def save_file():
    path = filedialog.asksaveasfilename(
        initialdir="Text Files",
        defaultextension=".txt",
        filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
    )
    if not path:
        return
    try:
        with open(path, "w", encoding="utf-8") as f:
            f.write(editor.text.get("1.0", "end-1c"))
    except Exception as e:
        messagebox.showerror("Save error", str(e))


# ---------------- GUI ----------------
root = tk.Tk()
root.title("Compiler GUI")

# Top buttons
topbar = tk.Frame(root)
topbar.pack(fill="x")

btn_run = tk.Button(topbar, text="Run", command=run_compiler)
btn_run.pack(side="left", padx=5, pady=5)

btn_load = tk.Button(topbar, text="Load .txt", command=load_file)
btn_load.pack(side="left", padx=5, pady=5)

btn_save = tk.Button(topbar, text="Save .txt", command=save_file)
btn_save.pack(side="left", padx=5, pady=5)

# Editor with line numbers
editor = LineNumberedText(root, width=100, height=22)
editor.pack(fill="both", expand=True)

# Output area
output_label = tk.Label(root, text="Output:")
output_label.pack(anchor="w", padx=5)

output = ScrolledText(root, width=100, height=16, wrap="word")
output.pack(fill="both", expand=False)

root.mainloop()
