import os
import re
import subprocess
import tkinter as tk
from tkinter import filedialog, messagebox
from tkinter.scrolledtext import ScrolledText
from tkinter import ttk

# ---------------- CONFIG ----------------
EXE = "./exe/compiler.exe"
DEFAULT_FILE = "Text Files/input.txt"

QUADS_TXT = "exe/quadruples.txt"
SYMTAB_TXT = "exe/scopes_logs.txt"
ASM_TXT = "exe/output.asm"
# ----------------------------------------


# ---------------- THEME (Dark Mode) ----------------
COL_BG = "#0f111a"
COL_PANEL = "#151826"
COL_PANEL2 = "#101322"
COL_TEXT_BG = "#0b0d14"
COL_TEXT_FG = "#d7dae0"
COL_MUTED = "#9aa3b2"
COL_ACCENT = "#6aa9ff"
COL_BORDER = "#2a2f45"
COL_ERROR = "#ff6b6b"
COL_WARN = "#ffd166"
COL_OK = "#7CFC98"

FONT_UI = ("Segoe UI", 10)
FONT_MONO = ("Consolas", 11)  # good on Windows/WSL; if missing it falls back


# ---------------- Helpers ----------------
def _ensure_dir(path: str):
    d = os.path.dirname(path)
    if d:
        os.makedirs(d, exist_ok=True)


def _read_text_file(path: str) -> str:
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        return f.read()


def _write_text_file(path: str, content: str):
    _ensure_dir(path)
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)


def _split_stdout_into_console_errors_warnings(stdout: str):
    """
    Your compiler prints blocks like:
        "\nN Errors:\n"
        "... error lines ..."

        "\nM Warnings:\n"
        "... warning lines ..."

    We'll split into:
      - console_out: everything before the first Errors/Warnings block
      - errors_out: the Errors block (including its header), if any
      - warnings_out: the Warnings block (including its header), if any
    """
    if not stdout:
        return "", "", ""

    err_m = re.search(r"\n\d+\s+Errors:\s*\n", stdout)
    warn_m = re.search(r"\n\d+\s+Warnings:\s*\n", stdout)

    if not err_m and not warn_m:
        return stdout, "", ""

    starts = []
    if err_m:
        starts.append(("err", err_m.start()))
    if warn_m:
        starts.append(("warn", warn_m.start()))
    starts.sort(key=lambda x: x[1])

    first_kind, first_pos = starts[0]
    console_out = stdout[:first_pos].rstrip()

    errors_out = ""
    warnings_out = ""

    def slice_block(start_pos, next_pos):
        return stdout[start_pos:next_pos].lstrip("\n").rstrip() + "\n"

    if err_m and warn_m:
        if err_m.start() < warn_m.start():
            errors_out = slice_block(err_m.start(), warn_m.start())
            warnings_out = slice_block(warn_m.start(), len(stdout))
        else:
            warnings_out = slice_block(warn_m.start(), err_m.start())
            errors_out = slice_block(err_m.start(), len(stdout))
    else:
        if err_m:
            errors_out = slice_block(err_m.start(), len(stdout))
        if warn_m:
            warnings_out = slice_block(warn_m.start(), len(stdout))

    return console_out, errors_out, warnings_out


ERROR_LINE_RE = re.compile(r"\bLine\s+(\d+)\b")


# ---------------- Editor with line numbers + highlighting ----------------
class LineNumberedEditor(tk.Frame):
    KEYWORDS = {
        "if", "else", "while", "repeat", "until", "for", "switch", "case",
        "const", "return", "continue", "break", "default", "function",
        "int", "float", "bool", "char", "string", "void",
        "true", "false"
    }

    def __init__(self, master):
        super().__init__(master, bg=COL_PANEL2)

        self.linenumbers = tk.Text(
            self,
            width=5,
            padx=6,
            takefocus=0,
            borderwidth=0,
            background=COL_PANEL2,
            foreground=COL_MUTED,
            state="disabled",
            wrap="none",
            font=FONT_MONO
        )
        self.linenumbers.pack(side="left", fill="y")

        self.text = ScrolledText(
            self,
            wrap="none",
            undo=True,
            font=FONT_MONO,
            background=COL_TEXT_BG,
            foreground=COL_TEXT_FG,
            insertbackground=COL_ACCENT,
            selectbackground="#2a3b66",
            borderwidth=1,
            relief="solid"
        )
        self.text.pack(side="right", fill="both", expand=True)

        # tags for highlighting
        self.text.tag_configure("kw", foreground=COL_ACCENT)
        self.text.tag_configure("string", foreground="#98c379")
        self.text.tag_configure("number", foreground="#f2a365")
        self.text.tag_configure("comment", foreground="#6b7280")
        self.text.tag_configure("current_line", background="#131a2a")

        # update line numbers + highlight (debounced)
        self._highlight_job = None
        self.text.bind("<KeyRelease>", self._on_change)
        self.text.bind("<ButtonRelease-1>", self._on_change)
        self.text.bind("<MouseWheel>", self._on_scroll)
        self.text.bind("<Button-4>", self._on_scroll)  # Linux scroll up
        self.text.bind("<Button-5>", self._on_scroll)  # Linux scroll down

        # keep current line highlight
        self.text.bind("<KeyRelease>", self._highlight_current_line, add=True)
        self.text.bind("<ButtonRelease-1>", self._highlight_current_line, add=True)

        self.after(120, self._refresh_all)

    def _on_scroll(self, event=None):
        self.update_linenumbers()
        self._highlight_current_line()

    def _on_change(self, event=None):
        self.update_linenumbers()
        self._highlight_current_line()
        if self._highlight_job is not None:
            self.after_cancel(self._highlight_job)
        self._highlight_job = self.after(120, self.apply_syntax_highlighting)

    def _refresh_all(self):
        self.update_linenumbers()
        self._highlight_current_line()
        self.apply_syntax_highlighting()
        self.after(250, self._refresh_all)

    def update_linenumbers(self, event=None):
        self.linenumbers.config(state="normal")
        self.linenumbers.delete("1.0", "end")

        start_index = self.text.index("@0,0")
        start_line = int(start_index.split(".")[0])

        height = self.text.winfo_height()
        line_info = self.text.dlineinfo("1.0")
        line_height = int(line_info[3]) if line_info else 15
        visible_lines = max(1, height // line_height)

        for i in range(start_line, start_line + visible_lines + 3):
            self.linenumbers.insert("end", f"{i}\n")

        self.linenumbers.config(state="disabled")

    def _highlight_current_line(self, event=None):
        self.text.tag_remove("current_line", "1.0", "end")
        try:
            line = self.text.index("insert").split(".")[0]
            self.text.tag_add("current_line", f"{line}.0", f"{line}.0 lineend+1c")
        except Exception:
            pass

    def goto_line(self, line_no: int):
        if line_no < 1:
            line_no = 1
        idx = f"{line_no}.0"
        self.text.focus_set()
        self.text.mark_set("insert", idx)
        self.text.see(idx)
        self._highlight_current_line()

    def apply_syntax_highlighting(self):
        for tag in ("kw", "string", "number", "comment"):
            self.text.tag_remove(tag, "1.0", "end")

        content = self.text.get("1.0", "end-1c")
        if not content:
            return

        for m in re.finditer(r"//.*?$", content, re.MULTILINE):
            start = f"1.0+{m.start()}c"
            end = f"1.0+{m.end()}c"
            self.text.tag_add("comment", start, end)

        for m in re.finditer(r"\"([^\"\\]|\\.)*\"", content):
            start = f"1.0+{m.start()}c"
            end = f"1.0+{m.end()}c"
            self.text.tag_add("string", start, end)

        for m in re.finditer(r"\b\d+(\.\d+)?\b", content):
            start = f"1.0+{m.start()}c"
            end = f"1.0+{m.end()}c"
            self.text.tag_add("number", start, end)

        for kw in sorted(self.KEYWORDS, key=len, reverse=True):
            for m in re.finditer(rf"\b{re.escape(kw)}\b", content):
                start = f"1.0+{m.start()}c"
                end = f"1.0+{m.end()}c"
                self.text.tag_add("kw", start, end)


# ---------------- Main GUI App ----------------
class CompilerGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Compiler GUI")
        self.configure(bg=COL_BG)

        self._apply_ttk_dark_theme()

        # Top toolbar
        self.toolbar = tk.Frame(self, bg=COL_PANEL, highlightbackground=COL_BORDER, highlightthickness=1)
        self.toolbar.pack(fill="x")

        self.btn_run = self._btn(self.toolbar, "Run", self.run_and_refresh)
        self.btn_run.pack(side="left", padx=6, pady=6)

        self.btn_load = self._btn(self.toolbar, "Load .txt", self.load_file)
        self.btn_load.pack(side="left", padx=6, pady=6)

        self.btn_save = self._btn(self.toolbar, "Save .txt", self.save_file)
        self.btn_save.pack(side="left", padx=6, pady=6)

        ttk.Separator(self.toolbar, orient="vertical").pack(side="left", fill="y", padx=8, pady=6)

        self.btn_console = self._btn(self.toolbar, "Console", lambda: self.select_tab("console"))
        self.btn_console.pack(side="left", padx=6, pady=6)

        self.btn_errors = self._btn(self.toolbar, "Errors", lambda: self.select_tab("errors"))
        self.btn_errors.pack(side="left", padx=6, pady=6)

        # NEW: Warnings button
        self.btn_warnings = self._btn(self.toolbar, "Warnings", lambda: self.select_tab("warnings"))
        self.btn_warnings.pack(side="left", padx=6, pady=6)

        self.btn_quads = self._btn(self.toolbar, "Quadruples", lambda: self.select_tab("quads"))
        self.btn_quads.pack(side="left", padx=6, pady=6)

        self.btn_symtab = self._btn(self.toolbar, "Symbol Table", lambda: self.select_tab("symtab"))
        self.btn_symtab.pack(side="left", padx=6, pady=6)

        self.btn_asm = self._btn(self.toolbar, "Assembly", lambda: self.select_tab("asm"))
        self.btn_asm.pack(side="left", padx=6, pady=6)

        self.btn_stderr = self._btn(self.toolbar, "StdErr", lambda: self.select_tab("stderr"))
        self.btn_stderr.pack(side="left", padx=6, pady=6)

        # Main split view
        self.paned = ttk.Panedwindow(self, orient="horizontal")
        self.paned.pack(fill="both", expand=True)

        # Left: editor
        self.left_frame = tk.Frame(self.paned, bg=COL_BG)
        self.editor = LineNumberedEditor(self.left_frame)
        self.editor.pack(fill="both", expand=True)
        self.paned.add(self.left_frame, weight=3)

        # Right: tabs
        self.right_frame = tk.Frame(self.paned, bg=COL_BG)
        self.nb = ttk.Notebook(self.right_frame)
        self.nb.pack(fill="both", expand=True, padx=8, pady=8)
        self.paned.add(self.right_frame, weight=2)

        # Notebook tab frames
        self.tab_frames = {}
        self.tab_frames["console"] = self._make_tab("Console")
        self.console_text = self._make_output_text(self.tab_frames["console"])

        self.tab_frames["errors"] = self._make_tab("Errors")
        self.errors_text = self._make_output_text(self.tab_frames["errors"])
        self._setup_clickable_errors()

        # NEW: Warnings tab
        self.tab_frames["warnings"] = self._make_tab("Warnings")
        self.warnings_text = self._make_output_text(self.tab_frames["warnings"])
        self.warnings_text.tag_configure("warnline", foreground=COL_WARN, underline=True)
        self.warnings_text.tag_configure("warnhdr", foreground=COL_WARN, font=FONT_MONO)

        self.tab_frames["quads"] = self._make_tab("Quadruples")
        self.quads_text = self._make_output_text(self.tab_frames["quads"])

        self.tab_frames["symtab"] = self._make_tab("Symbol Table")
        self.symtab_text = self._make_output_text(self.tab_frames["symtab"])

        self.tab_frames["asm"] = self._make_tab("Assembly")
        self.asm_text = self._make_output_text(self.tab_frames["asm"])

        self.tab_frames["stderr"] = self._make_tab("StdErr")
        self.stderr_text = self._make_output_text(self.tab_frames["stderr"])

        # Status bar
        self.status = tk.Label(self, text="Ready", bg=COL_PANEL, fg=COL_MUTED, anchor="w", padx=10)
        self.status.pack(fill="x")

        # initial content
        self.console_text.insert("1.0", "Console output will appear here.\n")
        self.select_tab("console")

    # ---------- UI helpers ----------
    def _apply_ttk_dark_theme(self):
        style = ttk.Style()
        try:
            style.theme_use("clam")
        except Exception:
            pass

        style.configure("TFrame", background=COL_BG)
        style.configure("TLabel", background=COL_BG, foreground=COL_TEXT_FG, font=FONT_UI)
        style.configure("TNotebook", background=COL_BG, borderwidth=0)
        style.configure("TNotebook.Tab", background=COL_PANEL, foreground=COL_TEXT_FG, padding=(10, 6))
        style.map("TNotebook.Tab", background=[("selected", COL_PANEL2)], foreground=[("selected", COL_ACCENT)])
        style.configure("TPanedwindow", background=COL_BG)

    def _btn(self, master, text, cmd):
        return tk.Button(
            master,
            text=text,
            command=cmd,
            bg=COL_PANEL2,
            fg=COL_TEXT_FG,
            activebackground=COL_BORDER,
            activeforeground=COL_TEXT_FG,
            relief="flat",
            padx=10,
            pady=6,
            font=FONT_UI,
            highlightthickness=1,
            highlightbackground=COL_BORDER,
            highlightcolor=COL_BORDER
        )

    def _make_tab(self, title: str) -> tk.Frame:
        f = tk.Frame(self.nb, bg=COL_BG)
        self.nb.add(f, text=title)
        return f

    def _make_output_text(self, parent: tk.Frame) -> ScrolledText:
        t = ScrolledText(
            parent,
            wrap="word",
            font=FONT_MONO,
            background=COL_TEXT_BG,
            foreground=COL_TEXT_FG,
            insertbackground=COL_ACCENT,
            selectbackground="#2a3b66",
            borderwidth=1,
            relief="solid"
        )
        t.pack(fill="both", expand=True, padx=10, pady=10)
        return t

    def select_tab(self, key: str):
        frame = self.tab_frames.get(key)
        if frame is not None:
            self.nb.select(frame)

    def _set_status(self, msg: str, kind: str = "info"):
        color = COL_MUTED
        if kind == "ok":
            color = COL_OK
        elif kind == "err":
            color = COL_ERROR
        elif kind == "warn":
            color = COL_WARN
        self.status.config(text=msg, fg=color)

    # ---------- compiler run ----------
    def _write_editor_to_temp_file(self) -> str:
        code = self.editor.text.get("1.0", "end-1c")
        _ensure_dir(DEFAULT_FILE)
        _write_text_file(DEFAULT_FILE, code)
        return DEFAULT_FILE

    def _run_compiler(self) -> subprocess.CompletedProcess:
        temp_file = self._write_editor_to_temp_file()
        if not os.path.exists(EXE):
            raise FileNotFoundError(f"Compiler executable not found: {EXE}")
        return subprocess.run([EXE, temp_file], text=True, capture_output=True)

    def _clear_outputs(self):
        for w in (self.console_text, self.errors_text, self.warnings_text,
                  self.quads_text, self.symtab_text, self.asm_text, self.stderr_text):
            w.delete("1.0", "end")

    def run_and_refresh(self):
        self._set_status("Running...", "warn")
        self._clear_outputs()

        try:
            p = self._run_compiler()
        except Exception as e:
            messagebox.showerror("Run error", str(e))
            self._set_status("Run failed", "err")
            return

        # Split stdout into console + errors + warnings
        console_out, errors_out, warnings_out = _split_stdout_into_console_errors_warnings(p.stdout or "")

        if console_out.strip():
            self.console_text.insert("1.0", console_out.strip() + "\n")
        else:
            self.console_text.insert("1.0", "(no console output)\n")

        if errors_out.strip():
            self.errors_text.insert("1.0", errors_out.strip() + "\n")
        else:
            self.errors_text.insert("1.0", "(no errors)\n")

        if warnings_out.strip():
            self.warnings_text.insert("1.0", warnings_out.strip() + "\n")
            self._tag_warning_lines()
        else:
            self.warnings_text.insert("1.0", "(no warnings)\n")

        if (p.stderr or "").strip():
            self.stderr_text.insert("1.0", p.stderr.strip() + "\n")
        else:
            self.stderr_text.insert("1.0", "(empty)\n")

        # Load artifacts
        self._load_artifact_into(self.quads_text, QUADS_TXT, "(quadruples file not found)")
        self._load_artifact_into(self.symtab_text, SYMTAB_TXT, "(symbol table file not found)")
        self._load_artifact_into(self.asm_text, ASM_TXT, "(assembly file not found)")

        # Choose default tab after run:
        if errors_out.strip():
            self.select_tab("errors")
            self._tag_error_lines()
            self._set_status("Finished with errors", "err")
        elif warnings_out.strip():
            self.select_tab("warnings")
            self._set_status("Finished with warnings", "warn")
        else:
            self.select_tab("console")
            self._set_status("Finished successfully", "ok")

    def _load_artifact_into(self, widget: ScrolledText, path: str, missing_msg: str):
        if os.path.exists(path):
            try:
                widget.insert("1.0", _read_text_file(path))
            except Exception as e:
                widget.insert("1.0", f"Failed to read {path}\n{e}\n")
        else:
            widget.insert("1.0", f"{missing_msg}\nExpected: {path}\n")

    # ---------- file ops ----------
    def load_file(self):
        path = filedialog.askopenfilename(
            initialdir="Text Files",
            title="Open file",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
        )
        if not path:
            return
        try:
            content = _read_text_file(path)
            self.editor.text.delete("1.0", "end")
            self.editor.text.insert("1.0", content)
        except Exception as e:
            messagebox.showerror("Open error", str(e))

    def save_file(self):
        path = filedialog.asksaveasfilename(
            initialdir="Text Files",
            defaultextension=".txt",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")]
        )
        if not path:
            return
        try:
            content = self.editor.text.get("1.0", "end-1c")
            _write_text_file(path, content)
            self._set_status(f"Saved: {path}", "ok")
        except Exception as e:
            messagebox.showerror("Save error", str(e))

    # ---------- Clickable error lines ----------
    def _setup_clickable_errors(self):
        self.errors_text.tag_configure("errline", foreground=COL_ERROR, underline=True)
        self.errors_text.tag_configure("errhdr", foreground=COL_WARN, font=FONT_MONO)
        self.errors_text.bind("<Button-1>", self._on_errors_click)

    def _tag_error_lines(self):
        self.errors_text.tag_remove("errline", "1.0", "end")
        self.errors_text.tag_remove("errhdr", "1.0", "end")

        text = self.errors_text.get("1.0", "end-1c")
        if not text:
            return

        for m in re.finditer(r"^\d+\s+Errors:\s*$", text, re.MULTILINE):
            s = f"1.0+{m.start()}c"
            e = f"1.0+{m.end()}c"
            self.errors_text.tag_add("errhdr", s, e)

        for m in ERROR_LINE_RE.finditer(text):
            pos = m.start()
            idx = f"1.0+{pos}c"
            line_start = self.errors_text.index(f"{idx} linestart")
            line_end = self.errors_text.index(f"{idx} lineend")
            self.errors_text.tag_add("errline", line_start, line_end)

    def _tag_warning_lines(self):
        self.warnings_text.tag_remove("warnline", "1.0", "end")
        self.warnings_text.tag_remove("warnhdr", "1.0", "end")

        text = self.warnings_text.get("1.0", "end-1c")
        if not text:
            return

        for m in re.finditer(r"^\d+\s+Warnings:\s*$", text, re.MULTILINE):
            s = f"1.0+{m.start()}c"
            e = f"1.0+{m.end()}c"
            self.warnings_text.tag_add("warnhdr", s, e)

        lines = text.splitlines()
        idx = 1
        for line in lines:
            line_start = f"{idx}.0"
            line_end = f"{idx}.0 lineend"
            if line.strip() and not re.match(r"^\d+\s+Warnings:\s*$", line.strip()):
                self.warnings_text.tag_add("warnline", line_start, line_end)
            idx += 1

    def _on_errors_click(self, event):
        idx = self.errors_text.index(f"@{event.x},{event.y}")
        tags = self.errors_text.tag_names(idx)
        if "errline" not in tags:
            return

        line_start = self.errors_text.index(f"{idx} linestart")
        line_end = self.errors_text.index(f"{idx} lineend")
        line_text = self.errors_text.get(line_start, line_end)

        m = ERROR_LINE_RE.search(line_text)
        if not m:
            return
        line_no = int(m.group(1))

        self.editor.goto_line(line_no)
        self.select_tab("errors")  # keep errors visible


if __name__ == "__main__":
    app = CompilerGUI()
    app.mainloop()