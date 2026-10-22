#!/usr/bin/env python3
"""
XCMessages GUI Editor

A small Python/Tkinter application for inspecting, editing, and saving
Xplicit Computing MESSAGES protobuf files.

Supported features:
  - Open binary protobuf files and JSON files.
  - Create blank messages from scratch.
  - Navigate nested and repeated message fields.
  - Edit scalar fields, string fields, bool fields, bytes fields,
    repeated scalar fields, and repeated message fields.
  - Save in the same format as the loaded file.
  - Inspect file-like string references and report file metadata.

This script must be run in a directory containing the generated bindings:
  vector_pb2.py
  spatial_pb2.py
  concept_pb2.py
  meta_pb2.py

or those files must be importable via PYTHONPATH.
"""
import os
import subprocess
import webbrowser
import argparse
import mimetypes
import sys
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Optional
from urllib.parse import urlparse

try:
    import tkinter as tk
    from tkinter import filedialog, messagebox, ttk
except Exception as exc:
    print("Failed to import tkinter:", exc)
    sys.exit(1)

try:
    from google.protobuf import json_format
except Exception as exc:
    print("Failed to import Google protobuf runtime:", exc)
    sys.exit(1)

try:
    import vector_pb2
    import spatial_pb2
    import concept_pb2
    import meta_pb2
except Exception as exc:
    print("Failed to import generated Messages bindings:", exc)
    print("Run this script in the folder containing:")
    print("  vector_pb2.py")
    print("  spatial_pb2.py")
    print("  concept_pb2.py")
    print("  meta_pb2.py")
    sys.exit(1)


# -----------------------------------------------------------------------------
# Protobuf descriptor constants.
# -----------------------------------------------------------------------------

TYPE_DOUBLE = 1
TYPE_FLOAT = 2
TYPE_INT64 = 3
TYPE_UINT64 = 4
TYPE_INT32 = 5
TYPE_FIXED64 = 6
TYPE_FIXED32 = 7
TYPE_BOOL = 8
TYPE_STRING = 9
TYPE_GROUP = 10
TYPE_MESSAGE = 11
TYPE_BYTES = 12
TYPE_UINT32 = 13
TYPE_SFIXED32 = 14
TYPE_SFIXED64 = 15
TYPE_SINT32 = 16
TYPE_SINT64 = 17

LABEL_REPEATED = 3

INT_TYPES = {
    TYPE_INT32,
    TYPE_UINT32,
    TYPE_INT64,
    TYPE_UINT64,
    TYPE_SINT32,
    TYPE_SINT64,
    TYPE_SFIXED32,
    TYPE_SFIXED64,
    TYPE_FIXED32,
    TYPE_FIXED64,
}

UINT_TYPES = {TYPE_UINT32, TYPE_UINT64}

NUMERIC_TYPES = INT_TYPES | {TYPE_FLOAT, TYPE_DOUBLE}

FILE_LIKE_FIELDS = {
    "file",
    "path",
    "filename",
    "directory",
    "url",
    "referenced_files",
    "resource_name",
    "symbol",
    "source_file",
}

REPEATED_UI_LIMIT = 5000

THEME = {
    "bg": "#0f141b",
    "panel": "#151c26",
    "field_bg": "#1c2532",
    "text": "#e8f0f7",
    "muted": "#a9b6c4",
    "accent": "#8ab4f8",
    "accent2": "#a3d9a5",
    "warn": "#f5d76e",
    "error": "#f2a2a2",
    "selection": "#2b5a8c",
}


# -----------------------------------------------------------------------------
# Small helpers.
# -----------------------------------------------------------------------------

@dataclass
class Step:
    """One navigation step into a repeated or non-repeated message field."""

    field: object
    index: Optional[int] = None


def build_type_registry() -> dict:
    """
    Build an ordered dictionary of message class names to protobuf classes.
    """

    modules = [
        vector_pb2,
        spatial_pb2,
        concept_pb2,
        meta_pb2,
    ]

    preferred = [
        "System",
        "Topology",
        "Meta",
        "Vector64",
        "Vector32",
        "Record64",
        "File",
        "Variables",
        "Script",
        "Model",
        "Function",
        "Command",
        "Coupling",
        "Zone",
        "GridControl",
        "MeshControl",
        "TreeControl",
        "Visual",
        "Representation",
        "GraphicsControl",
        "CameraControl",
        "SparseMatrix",
        "Elements",
        "UIndices",
        "Rule",
        "Extrema",
        "Revision",
        "Shape",
        "DirentNode",
        "SliceControl",
    ]

    found = {}
    for mod in modules:
        for name, obj in vars(mod).items():
            if name.startswith("_"):
                continue
            if not isinstance(obj, type):
                continue
            if not hasattr(obj, "DESCRIPTOR"):
                continue
            try:
                full_name = obj.DESCRIPTOR.full_name
            except Exception:
                continue
            if not full_name.startswith("Messages."):
                continue
            found[name] = obj

    ordered = {}
    for name in preferred:
        if name in found:
            ordered[name] = found.pop(name)

    for name, obj in found.items():
        ordered[name] = obj

    return ordered


def field_type_text(field) -> str:
    """Return a human-readable protobuf type string for a field."""

    base_map = {
        TYPE_DOUBLE: "double",
        TYPE_FLOAT: "float",
        TYPE_INT64: "int64",
        TYPE_UINT64: "uint64",
        TYPE_INT32: "int32",
        TYPE_FIXED64: "fixed64",
        TYPE_FIXED32: "fixed32",
        TYPE_BOOL: "bool",
        TYPE_STRING: "string",
        TYPE_GROUP: "group",
        TYPE_BYTES: "bytes",
        TYPE_UINT32: "uint32",
        TYPE_SFIXED32: "sfixed32",
        TYPE_SFIXED64: "sfixed64",
        TYPE_SINT32: "sint32",
        TYPE_SINT64: "sint64",
    }

    if field.type == TYPE_MESSAGE:
        base = field.message_type.name if field.message_type is not None else "message"
    else:
        base = base_map.get(field.type, str(field.type))

    if field.label == LABEL_REPEATED:
        return f"repeated {base}"
    return base


def coerce_value(field, text):
    """Convert user text to the Python type expected by a protobuf field."""

    text = str(text).strip()

    if field.type == TYPE_BOOL:
        return text.lower() in {"1", "true", "yes", "on"}

    if field.type == TYPE_STRING:
        return str(text)

    if field.type in INT_TYPES:
        value = int(text, 0)
        if field.type in UINT_TYPES and value < 0:
            raise ValueError("unsigned value cannot be negative")
        return value

    if field.type in (TYPE_FLOAT, TYPE_DOUBLE):
        return float(text)

    if field.type == TYPE_BYTES:
        return str(text).encode("utf-8")

    raise ValueError(f"Unsupported protobuf type: {field.type}")


def default_scalar_value(field):
    """Return a reasonable default value for adding a new repeated scalar."""

    if field.type == TYPE_STRING:
        return ""
    if field.type == TYPE_BOOL:
        return False
    if field.type in (TYPE_FLOAT, TYPE_DOUBLE):
        return 0.0
    if field.type == TYPE_BYTES:
        return b""
    return 0


def short_repr(value, limit: int = 80) -> str:
    """Return a short repr() string for display."""

    text = repr(value)
    if len(text) <= limit:
        return text
    return text[: max(0, limit - 3)] + "..."


def is_file_like_field(field, context_full_name: str) -> bool:
    """
    Heuristic: does this string field look like a file/system reference?
    """

    name = field.name

    if name in FILE_LIKE_FIELDS:
        return True

    # The File message's name field is a filename reference.
    if name == "name" and context_full_name == "Messages.File":
        return True

    # If a field is itself a File message, its name/filename are file-like.
    if field.message_type is not None and field.message_type.full_name == "Messages.File":
        if name in {"name", "filename", "file", "path"}:
            return True

    return False


def inspect_file_reference(
    value: str,
    field_name: str,
    context_full_name: str,
    base_dir: Optional[Path] = None,
) -> str:
    """
    Attempt to resolve a local file reference and return metadata text.
    """

    value = str(value or "").strip()
    if not value:
        return "No reference."

    # Network-ish URLs: report, but do not touch the network.
    if "://" in value:
        parsed = urlparse(value)
        scheme = parsed.scheme.lower()

        if scheme in {"http", "https", "ftp", "s3", "gs"}:
            return (
                f"URL reference: {value}\n"
                f"No local existence check performed.\n"
                f"Scheme: {scheme}"
            )

        if scheme == "file":
            candidate = Path(parsed.path).expanduser()
        else:
            return f"Unknown URL scheme: {value}"
    else:
        candidate = Path(value).expanduser()
        if not candidate.is_absolute():
            base = Path(base_dir) if base_dir is not None else Path.cwd()
            candidate = base / candidate

    if candidate.exists():
        try:
            stat_result = candidate.stat()
        except OSError as exc:
            return f"Exists: yes, but stat failed.\nPath: {candidate}\nError: {exc}"

        kind = "directory" if candidate.is_dir() else "file"
        mime_type = mimetypes.guess_type(str(candidate))[0]
        mime_text = f"\nMIME: {mime_type}" if mime_type else ""

        modified_text = datetime.fromtimestamp(stat_result.st_mtime).isoformat(timespec="seconds")

        return (
            f"Exists: yes\n"
            f"Path: {candidate}\n"
            f"Type: {kind}\n"
            f"Size: {stat_result.st_size:,} bytes\n"
            f"Modified: {modified_text}"
            f"{mime_text}"
        )

    return (
        f"Exists: no\n"
        f"Expected path: {candidate}\n"
        f"Base directory: {Path(base_dir) if base_dir is not None else Path.cwd()}"
    )


def guess_format(path: Path) -> str:
    """
    Guess whether a file should be parsed as JSON or binary protobuf.
    """

    path = Path(path)
    suffix = path.suffix.lower()

    if suffix == ".json":
        return "json"

    if suffix in {".xco", ".xcg", ".xcs", ".xcm", ".pb", ".bin", ".dat", ".raw"}:
        return "binary"

    try:
        with open(path, "rb") as fin:
            head = fin.read(1024)
        stripped = head.lstrip()
        if stripped[:1] in {b"{", b"["}:
            return "json"
    except Exception:
        pass

    return "binary"


def load_message(message_cls, path: Path):
    """
    Load a protobuf message from either binary or JSON.
    Returns (message, format).
    """

    path = Path(path)
    preferred = guess_format(path)
    candidates = [preferred]
    if preferred == "binary":
        candidates.append("json")
    else:
        candidates.append("binary")

    errors = []

    for fmt in candidates:
        msg = message_cls()
        try:
            if fmt == "json":
                text = path.read_text(encoding="utf-8-sig")
                try:
                    json_format.Parse(text, msg, ignore_unknown_fields=True)
                except TypeError:
                    # Older protobuf versions may not support ignore_unknown_fields.
                    json_format.Parse(text, msg)
            else:
                msg.ParseFromString(path.read_bytes())

            return msg, fmt

        except Exception as exc:
            errors.append(f"{fmt}: {exc}")

    raise RuntimeError("Could not parse file.\n" + "\n".join(errors))


def message_is_set(msg, field) -> bool:
    """Safely check whether a message field is populated."""

    try:
        return bool(msg.HasField(field.name))
    except Exception:
        return False


# -----------------------------------------------------------------------------
# Main GUI application.
# -----------------------------------------------------------------------------

class XCMessagesEditor(tk.Tk):
    def __init__(self, initial_type: str = "System", initial_file: Optional[str] = None):
        super().__init__()

        self.type_registry = build_type_registry()

        if initial_type not in self.type_registry:
            initial_type = "System"

        self.root_type_name = initial_type
        self.root_cls = self.type_registry[self.root_type_name]
        self.root_msg = self.root_cls()

        self.load_path: Optional[Path] = None
        self.input_format: Optional[str] = None
        self.base_dir: Path = Path.cwd()

        self.path = []
        self.dirty = False

        self.field_by_iid = {}

        # Editor widget references, cleared when switching fields.
        self.rm_field = None
        self.rlist = None
        self.rm_hint = None

        self.rs_field = None
        self.rs_values = None
        self.rs_list = None
        self.rs_summary = None
        self.rs_file_info = None
        self.rs_hint = None
        self.rs_large = False
        self.rs_index_var = None
        self.rs_value_var = None

        self._build_ui()
        self._show_current_message()

        if initial_file:
            self.open_file(Path(initial_file))

        self.protocol("WM_DELETE_WINDOW", self._on_close)

    # -------------------------------------------------------------------
    # UI construction
    # -------------------------------------------------------------------

    def _apply_theme(self):
        self.configure(bg=THEME["bg"])

        self.option_add("*Toplevel.background", THEME["bg"])
        self.option_add("*Frame.background", THEME["bg"])

        self.option_add("*Menu.background", THEME["panel"])
        self.option_add("*Menu.foreground", THEME["text"])
        self.option_add("*Menu.activeBackground", THEME["selection"])
        self.option_add("*Menu.activeForeground", THEME["text"])
        self.option_add("*Teardrop.background", THEME["panel"])

        self.option_add("*Checkbutton.background", THEME["bg"])
        self.option_add("*Checkbutton.foreground", THEME["text"])
        self.option_add("*Checkbutton.activebackground", THEME["bg"])
        self.option_add("*Checkbutton.activeforeground", THEME["text"])
        self.option_add("*Checkbutton.selectcolor", THEME["field_bg"])

        for widget_name in ("Entry", "Text", "Listbox"):
            self.option_add(f"*{widget_name}.background", THEME["field_bg"])
            self.option_add(f"*{widget_name}.foreground", THEME["text"])
            self.option_add(f"*{widget_name}.insertBackground", THEME["text"])
            self.option_add(f"*{widget_name}.selectBackground", THEME["selection"])
            self.option_add(f"*{widget_name}.selectForeground", THEME["text"])

        style = ttk.Style(self)
        try:
            style.theme_use("clam")
        except Exception:
            pass

        style.configure(
            ".",
            background=THEME["bg"],
            foreground=THEME["text"],
            fieldbackground=THEME["field_bg"],
            bordercolor=THEME["panel"],
            lightcolor=THEME["panel"],
            darkcolor=THEME["panel"],
            troughcolor=THEME["panel"],
        )

        style.configure("TFrame", background=THEME["bg"])
        style.configure("TLabel", background=THEME["bg"], foreground=THEME["text"])

        style.configure(
            "TButton",
            background=THEME["panel"],
            foreground=THEME["text"],
            bordercolor=THEME["panel"],
            focusthickness=1,
            focuscolor=THEME["accent"],
            padding=(5, 2),
        )
        style.map(
            "TButton",
            background=[("active", THEME["accent"]), ("pressed", THEME["selection"])],
            foreground=[("active", THEME["bg"])],
        )

        style.configure("TCheckbutton", background=THEME["bg"], foreground=THEME["text"])
        style.map("TCheckbutton", background=[("active", THEME["bg"])])

        style.configure("TRadiobutton", background=THEME["bg"], foreground=THEME["text"])

        style.configure(
            "TCombobox",
            fieldbackground=THEME["field_bg"],
            background=THEME["panel"],
            foreground=THEME["text"],
            arrowcolor=THEME["accent"],
            bordercolor=THEME["panel"],
        )
        style.map(
            "TCombobox",
            fieldbackground=[("readonly", THEME["field_bg"]), ("focus", THEME["field_bg"])],
            foreground=[("readonly", THEME["text"]), ("focus", THEME["text"])],
            bordercolor=[("focus", THEME["accent"])],
        )

        style.configure(
            "Treeview",
            background=THEME["field_bg"],
            fieldbackground=THEME["field_bg"],
            foreground=THEME["text"],
            rowheight=24,
            borderwidth=0,
        )
        style.configure(
            "Treeview.Heading",
            background=THEME["panel"],
            foreground=THEME["accent"],
            relief="flat",
        )
        style.map(
            "Treeview",
            background=[("selected", THEME["selection"])],
            foreground=[("selected", THEME["text"])],
        )

        style.configure("TPanedwindow", background=THEME["bg"])
        style.configure("TSeparator", background=THEME["panel"])

    def _build_ui(self):
        self._apply_theme()
        self.title("XCMessages Editor")
        self.geometry("1280x820")

        # Menu bar.
        menu = tk.Menu(self)

        file_menu = tk.Menu(menu, tearoff=0)
        file_menu.add_command(label="New...", command=self.new_project)
        file_menu.add_command(label="Open...", command=self.open_file_dialog)
        file_menu.add_command(label="Save", command=self.save)
        file_menu.add_command(label="Save As...", command=self.save_as)
        file_menu.add_separator()
        file_menu.add_command(label="Scan File Dependencies", command=self.scan_file_dependencies)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self._on_close)
        menu.add_cascade(label="File", menu=file_menu)

        help_menu = tk.Menu(menu, tearoff=0)
        help_menu.add_command(
            label="About",
            command=lambda: messagebox.showinfo(
                "About",
                "XCMessages GUI Editor\n\n"
                "Open, navigate, edit, and save Xplicit Computing MESSAGES protobuf files.",
            ),
        )
        menu.add_cascade(label="Help", menu=help_menu)

        self.config(menu=menu)

        # Toolbar.
        toolbar = ttk.Frame(self)
        toolbar.pack(side=tk.TOP, fill=tk.X, padx=4, pady=2)

        ttk.Button(toolbar, text="New", command=self.new_project).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Open...", command=self.open_file_dialog).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Save", command=self.save).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="References", command=self.scan_file_dependencies).pack(side=tk.LEFT, padx=2)

        self.format_label = ttk.Label(toolbar, text="format: new")
        self.format_label.pack(side=tk.LEFT, padx=10)

        # Breadcrumb.
        self.breadcrumb_frame = ttk.Frame(self)
        self.breadcrumb_frame.pack(side=tk.TOP, fill=tk.X, padx=4, pady=0)

        # Main paned area.
        self.paned = ttk.PanedWindow(self, orient=tk.HORIZONTAL)
        self.paned.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=4, pady=2)

        tree_frame = ttk.Frame(self.paned)
        self.paned.add(tree_frame, weight=1)

        self.tree = ttk.Treeview(
            tree_frame,
            columns=("type", "value"),
            show="tree headings",
        )

        self.tree.heading("#0", text="Field")
        self.tree.heading("type", text="Type")
        self.tree.heading("value", text="Value")

        self.tree.column("#0", width=180)
        self.tree.column("type", width=140)
        self.tree.column("value", width=260)

        tree_scroll = ttk.Scrollbar(tree_frame, orient="vertical", command=self.tree.yview)
        self.tree.configure(yscrollcommand=tree_scroll.set)

        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tree_scroll.pack(side=tk.RIGHT, fill=tk.Y)

        self.tree.bind("<<TreeviewSelect>>", self._on_tree_select)
        self.tree.bind("<Double-Button-1>", self._on_tree_double_click)

        self.editor_frame = ttk.Frame(self.paned)
        self.paned.add(self.editor_frame, weight=2)

        # Status bar.
        self.status_var = tk.StringVar(value="Ready")
        status = ttk.Label(
            self,
            textvariable=self.status_var,
            relief=tk.SUNKEN,
            anchor=tk.W,
            padding=(4, 1),
        )
        status.pack(side=tk.BOTTOM, fill=tk.X)

    # -------------------------------------------------------------------
    # File operations
    # -------------------------------------------------------------------

    def new_project(self, type_name: Optional[str] = None, force: bool = False):
        if not force:
            if self.dirty and not messagebox.askyesno(
                "Unsaved changes",
                "Discard unsaved changes and start a new blank message?",
            ):
                return

        name = type_name or self.root_type_name or "System"
        if name not in self.type_registry:
            name = "System"

        self.root_type_name = name
        self.root_cls = self.type_registry[name]
        self.root_msg = self.root_cls()

        self.load_path = None
        self.input_format = None
        self.base_dir = Path.cwd()
        self.path = []
        self.dirty = False

        self.format_label.config(text="format: new")

        self._show_current_message()
        self._update_status()
        self._update_title()

    def open_file_dialog(self):
        path = filedialog.askopenfilename(
            title="Open XCMessages file",
            filetypes=[
                ("All files", "*.*"),
                ("JSON", "*.json"),
                ("XCMessages binary", "*.xco *.xcg *.xcs *.xcm *.pb *.bin *.dat"),
            ],
        )
        if path:
            self.open_file(Path(path))

    def open_file(self, path: Path):
        path = Path(path)

        if not path.exists():
            messagebox.showerror("Open failed", f"File does not exist:\n{path}")
            return

        if self.dirty and not messagebox.askyesno(
            "Unsaved changes",
            "Discard unsaved changes and open another file?",
        ):
            return

        type_name = self._guess_type(path) or self.root_type_name or "System"
        if type_name not in self.type_registry:
            type_name = "System"

        try:
            msg, fmt = load_message(self.type_registry[type_name], path)
        except Exception as exc:
            first_error = str(exc)
            chosen = self._ask_type_dialog(
                initial=type_name,
                message=(
                    f"Could not parse {path} as {type_name}.\n\n"
                    "Choose a different root message type:"
                ),
            )

            if not chosen:
                return

            try:
                msg, fmt = load_message(self.type_registry[chosen], path)
            except Exception as exc2:
                messagebox.showerror(
                    "Open failed",
                    f"Failed as {type_name}:\n{first_error}\n\n"
                    f"Failed as {chosen}:\n{exc2}",
                )
                return

            type_name = chosen

        self.root_cls = self.type_registry[type_name]
        self.root_msg = msg
        self.root_type_name = type_name

        self.load_path = path
        self.input_format = fmt
        self.base_dir = path.parent
        self.path = []
        self.dirty = False

        self.format_label.config(text=f"format: {fmt}")
        self._show_current_message()
        self._update_status()
        self._update_title()

    def save(self):
        if self.root_msg is None:
            return

        if self.load_path is None or self.input_format is None:
            self.save_as()
            return

        self._write(self.root_msg, self.load_path, self.input_format)

    def save_as(self):
        if self.root_msg is None:
            return

        ext_map = {
            "Vector64": ".xco",
            "Vector32": ".xco",
            "Topology": ".xcg",
            "System": ".xcs",
            "Meta": ".xcm",
        }

        if self.load_path is not None:
            default_name = self.load_path.name
        else:
            default_name = f"new_{self.root_cls.__name__.lower()}.pb"

        default_name = Path(default_name)

        if self.input_format == "json" or default_name.suffix.lower() == ".json":
            default_name = default_name.with_suffix(".json")
        else:
            if default_name.suffix.lower() == ".json":
                default_name = default_name.with_suffix(".pb")
            elif not default_name.suffix:
                default_name = default_name.with_suffix(ext_map.get(self.root_cls.__name__, ".pb"))

        path = filedialog.asksaveasfilename(
            title="Save XCMessages file",
            initialfile=str(default_name),
            defaultextension=str(default_name.suffix),
            filetypes=[
                ("Protobuf binary", "*.xco *.xcg *.xcs *.xcm *.pb *.bin *.dat"),
                ("JSON", "*.json"),
                ("All files", "*.*"),
            ],
        )

        if not path:
            return

        self._write(self.root_msg, Path(path), guess_format(Path(path)))

    def _write(self, msg, path: Path, fmt: str):
        path = Path(path)

        try:
            path.parent.mkdir(parents=True, exist_ok=True)

            if fmt == "json":
                try:
                    text = json_format.MessageToJson(
                        msg,
                        preserving_proto_field_name=True,
                        indent=2,
                    )
                except TypeError:
                    text = json_format.MessageToJson(msg, preserving_proto_field_name=True)
                path.write_text(text, encoding="utf-8")
            else:
                path.write_bytes(msg.SerializeToString())

            self.load_path = path
            self.input_format = fmt
            self.dirty = False
            self.format_label.config(text=f"format: {fmt}")
            self._update_status()
            self._update_title()

            messagebox.showinfo("Saved", f"Saved {path} as {fmt}.")

        except Exception as exc:
            messagebox.showerror("Save failed", str(exc))

    # -------------------------------------------------------------------
    # Type selection and dialogs
    # -------------------------------------------------------------------

    def _guess_type(self, path: Path) -> Optional[str]:
        mapping = {
            ".xco": "Vector64",
            ".xcg": "Topology",
            ".xcs": "System",
            ".xcm": "Meta",
        }

        suffix = Path(path).suffix.lower()
        if suffix in mapping and mapping[suffix] in self.type_registry:
            return mapping[suffix]

        return None

    def _ask_type_dialog(
        self,
        initial: str = "System",
        message: str = "Choose root message type:",
    ) -> Optional[str]:
        keys = list(self.type_registry)

        win = tk.Toplevel(self)
        win.configure(bg=THEME["bg"])
        win.title("Choose message type")
        win.transient(self)
        win.grab_set()

        ttk.Label(win, text=message, wraplength=520, justify=tk.LEFT).pack(
            padx=10,
            pady=6,
            anchor=tk.W,
        )

        listbox = tk.Listbox(win, width=36, height=18)
        for key in keys:
            listbox.insert(tk.END, key)
        listbox.pack(padx=10, pady=4)

        if initial in keys:
            listbox.selection_set(keys.index(initial))
            listbox.see(keys.index(initial))

        result = {"value": None}

        def ok():
            sel = listbox.curselection()
            result["value"] = keys[sel[0]] if sel else initial
            win.destroy()

        def cancel():
            result["value"] = None
            win.destroy()

        buttons = ttk.Frame(win)
        buttons.pack(pady=6)
        ttk.Button(buttons, text="OK", command=ok).pack(side=tk.LEFT, padx=4)
        ttk.Button(buttons, text="Cancel", command=cancel).pack(side=tk.LEFT, padx=4)

        win.protocol("WM_DELETE_WINDOW", cancel)
        win.wait_window()

        return result["value"]

    # -------------------------------------------------------------------
    # Navigation
    # -------------------------------------------------------------------

    def current_message(self):
        msg = self.root_msg

        for step in self.path:
            container = getattr(msg, step.field.name)
            if step.index is None:
                msg = container
            else:
                msg = container[step.index]

        return msg

    def _push_field(self, field, index: Optional[int] = None):
        self.path.append(Step(field, index))
        self._show_current_message()

    def _jump_to(self, level: int):
        self.path = self.path[:level]
        self._show_current_message()

    def _show_current_message(self):
        self._update_breadcrumb()

        for item in self.tree.get_children():
            self.tree.delete(item)

        self.field_by_iid.clear()

        msg = self.current_message()
        fields = list(msg.DESCRIPTOR.fields)

        if not fields:
            self.tree.insert("", "end", text="No fields", values=("empty",))
            self._clear_editor(hint=True)
        else:
            for field in fields:
                iid = self.tree.insert(
                    "",
                    "end",
                    text=field.name,
                    values=(field_type_text(field), self._preview_field(msg, field)),
                )
                self.field_by_iid[iid] = field

            self._clear_editor(hint=True)

        self._update_status()

    def _update_breadcrumb(self):
        for widget in self.breadcrumb_frame.winfo_children():
            widget.destroy()

        root_text = self.root_cls.__name__
        if len(root_text) > 18:
            root_text = root_text[:15] + "..."

        ttk.Button(
            self.breadcrumb_frame,
            text=root_text,
            width=18,
            command=lambda: self._jump_to(0),
        ).pack(side=tk.LEFT, padx=2, pady=1)

        for i, step in enumerate(self.path):
            if step.index is None:
                label = step.field.name
            else:
                label = f"{step.field.name}[{step.index}]"

            if len(label) > 22:
                label = label[:19] + "..."

            ttk.Button(
                self.breadcrumb_frame,
                text=label,
                width=22,
                command=lambda i=i: self._jump_to(i + 1),
            ).pack(side=tk.LEFT, padx=2, pady=1)

    def _on_tree_select(self, _event):
        sel = self.tree.selection()
        if not sel:
            return

        field = self.field_by_iid.get(sel[0])
        if field is not None:
            self._show_field_editor(field)

    # -------------------------------------------------------------------
    # Field preview and summaries
    # -------------------------------------------------------------------

    def _preview_field(self, msg, field) -> str:
        try:
            if field.label == LABEL_REPEATED:
                values = getattr(msg, field.name)
                count = len(values)

                if count == 0:
                    return "0 items"

                if field.type == TYPE_MESSAGE:
                    first = self._message_preview(values[0])
                    return f"{count} items; first={short_repr(first, 50)}"

                return f"{count} values; first={short_repr(values[0], 40)}"

            if field.type == TYPE_MESSAGE:
                return "set" if message_is_set(msg, field) else "empty"

            if field.type == TYPE_BYTES:
                data = bytes(getattr(msg, field.name) or b"")
                return f"{len(data):,} bytes"

            return short_repr(getattr(msg, field.name), 80)

        except Exception:
            return ""

    def _message_preview(self, msg) -> str:
        try:
            full_name = msg.DESCRIPTOR.full_name

            if full_name == "Messages.File":
                name = short_repr(msg.name, 40)
                size = len(bytes(msg.payload or b""))
                return f"File({name}, {size}B)"

            fields = msg.ListFields()
            if not fields:
                return "empty"

            parts = []
            for field, value in fields[:3]:
                if field.label == LABEL_REPEATED:
                    parts.append(f"{field.name}={len(value)}")
                elif field.type == TYPE_BYTES:
                    parts.append(f"{field.name}={len(value)}B")
                else:
                    parts.append(f"{field.name}={short_repr(value, 24)}")

            if len(fields) > 3:
                parts.append("...")

            return ", ".join(parts)

        except Exception:
            return type(msg).__name__

    def _update_field_preview(self, field):
        for iid, f in self.field_by_iid.items():
            if f is field:
                self.tree.set(iid, "value", self._preview_field(self.current_message(), f))
                break

    # -------------------------------------------------------------------
    # Editor framework
    # -------------------------------------------------------------------

    def _editor_hint_text(self):
        try:
            msg = self.current_message()
        except Exception:
            return "No message is loaded."

        lines = [f"Current message: {msg.DESCRIPTOR.name}"]

        if not msg.DESCRIPTOR.fields:
            lines.append("This message has no fields.")
        else:
            lines.append("Select a field from the list to inspect or edit it.")
            lines.append("Double-click a message field to open it.")
            lines.append("Double-click file-like string fields to open referenced files.")

        return "\n\n".join(lines)

    def _clear_editor(self, hint=False):
        for widget in self.editor_frame.winfo_children():
            widget.destroy()

        self.rm_field = None
        self.rlist = None
        self.rm_hint = None

        self.rs_field = None
        self.rs_values = None
        self.rs_list = None
        self.rs_summary = None
        self.rs_hint = None
        self.rs_file_info = None
        self.rs_large = False
        self.rs_index_var = None
        self.rs_value_var = None

        if hint:
            ttk.Label(
                self.editor_frame,
                text=self._editor_hint_text(),
                anchor=tk.N,
                justify=tk.LEFT,
                wraplength=560,
            ).pack(expand=True, padx=20, pady=20)

    def _show_field_editor(self, field):
        self._clear_editor(hint=False)
        container = self.current_message()

        ttk.Label(
            self.editor_frame,
            text=f"{container.DESCRIPTOR.full_name}::{field.name}",
            font=("Helvetica", 12, "bold"),
        ).pack(anchor=tk.W, padx=5, pady=(6, 0))

        if field.label == LABEL_REPEATED:
            self._create_repeated_editor(field, container)
        else:
            self._create_single_editor(field, container)

    def _on_tree_double_click(self, event):
        item = self.tree.identify_row(event.y)
        if not item:
            return

        field = self.field_by_iid.get(item)
        if field is None:
            return

        if field.type == TYPE_MESSAGE:
            if field.label == LABEL_REPEATED:
                values = getattr(self.current_message(), field.name)
                if not values:
                    messagebox.showinfo(
                        "Open",
                        "No embedded items yet.\n\nSelect this field and click Add."
                    )
                else:
                    self._push_field(field, 0)
            else:
                self._push_field(field)
            return

        if field.type == TYPE_STRING:
            if field.label == LABEL_REPEATED:
                values = getattr(self.current_message(), field.name)

                idx = 0
                if self.rs_field is field and self.rs_values is not None:
                    idx = self._rs_index()

                if 0 <= idx < len(values):
                    self._open_contextual_string(field, values[idx])
                elif len(values) > 0:
                    self._open_contextual_string(field, values[0])
                else:
                    messagebox.showinfo("Open", "This repeated string field is empty.")
            else:
                self._open_contextual_string(field, getattr(self.current_message(), field.name))

    def _on_repeated_scalar_double_click(self):
        if self.rs_list is None or self.rs_field is None or self.rs_values is None:
            return

        sel = self.rs_list.curselection()
        if not sel:
            return

        idx = sel[0]
        if not (0 <= idx < len(self.rs_values)):
            return

        self._open_contextual_string(self.rs_field, self.rs_values[idx])

    def _on_repeated_message_double_click(self):
        if self.rlist is None or self.rm_field is None:
            return

        sel = self.rlist.curselection()
        if not sel:
            return

        idx = sel[0]
        values = getattr(self.current_message(), self.rm_field.name)

        if not (0 <= idx < len(values)):
            return

        self._push_field(self.rm_field, idx)

    def _open_contextual_string(self, field, value):
        value = str(value or "").strip()
        if not value:
            messagebox.showinfo("Open", "No value to open.")
            return

        context = self.current_message()

        if field.name == "children" and context.DESCRIPTOR.full_name == "Messages.System":
            path = self._resolve_child_system_path(value, context)

            if path is None:
                messagebox.showerror(
                    "Child system not found",
                    f"Could not find child system file for:\n{value}\n\n"
                    "Looked for .xcs and .json files in the system directory "
                    "and possible child-system directory."
                )
            else:
                self.open_file(path)
            return

        if is_file_like_field(field, context.DESCRIPTOR.full_name):
            self._open_file_reference(value)
            return

        path = self._local_path_for_reference(value)
        if path is not None and path.exists():
            self._open_with_default(path)
        else:
            messagebox.showinfo("Open", "This string is not a recognized file reference.")

    def _local_path_for_reference(self, value):
        value = str(value or "").strip()
        if not value:
            return None

        if "://" in value:
            parsed = urlparse(value)
            if parsed.scheme.lower() == "file":
                return Path(parsed.path).expanduser()
            return None

        path = Path(value).expanduser()
        if not path.is_absolute():
            path = self.base_dir / path

        return path

    def _open_file_reference(self, value):
        value = str(value or "").strip()
        if not value:
            return

        if "://" in value:
            parsed = urlparse(value)
            if parsed.scheme.lower() in {"http", "https"}:
                webbrowser.open(value)
                return

        path = self._local_path_for_reference(value)

        if path is None:
            webbrowser.open(value)
            return

        if not path.exists():
            messagebox.showerror("File not found", f"File not found:\n{path}")
            return

        if path.suffix.lower() == ".xcg":
            self.open_file(path)
        else:
            self._open_with_default(path)

    def _open_with_default(self, path):
        try:
            if sys.platform.startswith("win"):
                os.startfile(str(path))  # type: ignore[attr-defined]
            elif sys.platform == "darwin":
                subprocess.Popen(["open", str(path)])
            else:
                subprocess.Popen(["xdg-open", str(path)])
        except Exception as exc:
            messagebox.showerror(
                "Open failed",
                f"Could not open with default application:\n{path}\n\n{exc}"
            )

    def _resolve_child_system_path(self, child_name, system_msg=None):
        child_name = str(child_name or "").strip()
        if not child_name:
            return None

        direct = Path(child_name).expanduser()
        direct_has_ext = direct.suffix.lower() in {".xcs", ".json"}

        if direct_has_ext:
            if direct.exists():
                return direct

            if not direct.is_absolute():
                cand = self.base_dir / direct
                if cand.exists():
                    return cand

        base_dirs = [self.base_dir]

        if system_msg is not None and system_msg.DESCRIPTOR.full_name == "Messages.System":
            dir_name = str(system_msg.meta.directory or "").strip()
            if dir_name:
                d = Path(dir_name).expanduser()
                if not d.is_absolute():
                    d = self.base_dir / d
                base_dirs.insert(0, d)

        seen = set()
        candidates = []

        for base in base_dirs:
            if direct_has_ext:
                cand = base / child_name
                resolved = str(cand)
                if resolved not in seen:
                    seen.add(resolved)
                    candidates.append(cand)
            else:
                for root in (base / child_name, base):
                    for suffix in (".xcs", ".json"):
                        cand = root / (child_name + suffix)
                        resolved = str(cand)
                        if resolved not in seen:
                            seen.add(resolved)
                            candidates.append(cand)

        for cand in candidates:
            if cand.exists():
                return cand

        return None

    # -------------------------------------------------------------------
    # Single-field editors
    # -------------------------------------------------------------------

    def _create_single_editor(self, field, container):
        frame = ttk.Frame(self.editor_frame)
        frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        actions = ttk.Frame(frame)
        actions.pack(side=tk.TOP, fill=tk.X, pady=(0, 4))

        if field.type == TYPE_MESSAGE:
            sub = getattr(container, field.name)
            set_fields = sub.ListFields()

            content = ttk.Frame(frame)
            content.pack(fill=tk.BOTH, expand=True)

            ttk.Label(content, text="Message field:").pack(anchor=tk.W)
            ttk.Label(
                content,
                text=f"Populated: {'yes' if message_is_set(container, field) else 'no'}"
            ).pack(anchor=tk.W, pady=2)
            ttk.Label(content, text=f"Fields set: {len(set_fields)}").pack(anchor=tk.W)
            ttk.Label(
                content,
                text="Double-click this field in the list or click Open to edit it."
            ).pack(anchor=tk.W, pady=(0, 6))

            ttk.Button(actions, text="Open", command=lambda: self._push_field(field)).pack(
                side=tk.LEFT,
                padx=2
            )
            ttk.Button(
                actions,
                text="Clear",
                command=lambda: self._clear_single_message(field, container)
            ).pack(side=tk.LEFT, padx=2)
            return

        if field.type == TYPE_BOOL:
            var = tk.BooleanVar(value=bool(getattr(container, field.name)))

            content = ttk.Frame(frame)
            content.pack(fill=tk.BOTH, expand=True)

            tk.Checkbutton(
                content,
                text="true",
                variable=var,
                command=lambda: self._set_bool(field, container, var)
            ).pack(anchor=tk.W)

            def clear_bool():
                var.set(False)
                self._set_bool(field, container, var)

            ttk.Button(actions, text="Clear", command=clear_bool).pack(side=tk.LEFT, padx=2)
            return

        if field.type == TYPE_BYTES:
            self._create_bytes_editor(field, container, frame, actions)
            return

        if field.type == TYPE_STRING:
            self._create_string_editor(field, container, frame, actions)
            return

        var = tk.StringVar(value=str(getattr(container, field.name)))

        content = ttk.Frame(frame)
        content.pack(fill=tk.BOTH, expand=True)

        ttk.Label(content, text="Value:").pack(anchor=tk.W)
        entry = ttk.Entry(content, textvariable=var, width=52)
        entry.pack(anchor=tk.W, pady=2)

        def apply(_event=None):
            self._set_scalar(field, container, var)

        entry.bind("<Return>", apply)

        def clear_scalar():
            default = default_scalar_value(field)
            var.set(str(default))
            self._set_scalar(field, container, var)

        ttk.Button(actions, text="Set", command=apply).pack(side=tk.LEFT, padx=2)
        ttk.Button(actions, text="Clear", command=clear_scalar).pack(side=tk.LEFT, padx=2)

        entry.focus_set()

    def _set_bool(self, field, container, var: tk.BooleanVar):
        try:
            setattr(container, field.name, bool(var.get()))
        except Exception as exc:
            messagebox.showerror("Set failed", str(exc))
            return

        self.mark_dirty()
        self._update_field_preview(field)

    def _set_scalar(self, field, container, var: tk.StringVar):
        try:
            value = coerce_value(field, var.get())
        except Exception as exc:
            messagebox.showerror("Invalid value", str(exc))
            return

        try:
            setattr(container, field.name, value)
        except Exception as exc:
            messagebox.showerror("Set failed", str(exc))
            return

        self.mark_dirty()
        self._update_field_preview(field)

    def _clear_single_message(self, field, container):
        if not messagebox.askyesno("Clear", f"Clear message field {field.name}?"):
            return

        try:
            container.ClearField(field.name)
        except Exception as exc:
            messagebox.showerror("Clear failed", str(exc))
            return

        self.mark_dirty()
        self._update_field_preview(field)
        self._show_field_editor(field)

    def _create_string_editor(self, field, container, frame, actions):
        value = str(getattr(container, field.name) or "")
        file_like = is_file_like_field(field, container.DESCRIPTOR.full_name)

        content = ttk.Frame(frame)
        content.pack(fill=tk.BOTH, expand=True, pady=(0, 4))

        info = None
        if file_like:
            info = ttk.Label(
                frame,
                text="",
                anchor=tk.W,
                justify=tk.LEFT,
                wraplength=760
            )
            info.pack(fill=tk.X, pady=2)

        text_widget = None
        var = None

        def current_value():
            if text_widget is not None:
                return text_widget.get("1.0", "end-1c")
            return var.get()

        def commit(_event=None):
            new_value = current_value()
            try:
                setattr(container, field.name, new_value)
            except Exception as exc:
                messagebox.showerror("Set failed", str(exc))
                return

            self.mark_dirty()
            self._update_field_preview(field)

            if info is not None:
                info.config(
                    text=inspect_file_reference(
                        new_value,
                        field.name,
                        container.DESCRIPTOR.full_name,
                        self.base_dir
                    )
                )

        def clear_string():
            if text_widget is not None:
                text_widget.delete("1.0", tk.END)
            else:
                var.set("")
            commit()

        if len(value) > 200 or "\n" in value:
            text_widget = tk.Text(content, height=12, width=90)
            text_widget.insert(tk.END, value)
            text_widget.pack(fill=tk.BOTH, expand=True)
            text_widget.bind("<Control-Return>", commit)
        else:
            var = tk.StringVar(value=value)

            top = ttk.Frame(content)
            top.pack(fill=tk.X)

            entry = ttk.Entry(top, textvariable=var, width=80)
            entry.pack(side=tk.LEFT, fill=tk.X, expand=True)
            entry.bind("<Return>", commit)

        ttk.Button(
            actions,
            text="Apply" if text_widget is not None else "Set",
            command=commit
        ).pack(side=tk.LEFT, padx=2)

        ttk.Button(actions, text="Clear", command=clear_string).pack(side=tk.LEFT, padx=2)

        if file_like:
            def browse():
                path = filedialog.askopenfilename(title="Select referenced file")
                if not path:
                    return

                if text_widget is not None:
                    text_widget.delete("1.0", tk.END)
                    text_widget.insert(tk.END, path)
                else:
                    var.set(path)

                commit()

            def open_reference():
                current = current_value()
                if not current.strip():
                    messagebox.showinfo("Open", "No value to open.")
                    return

                try:
                    setattr(container, field.name, current)
                except Exception as exc:
                    messagebox.showerror("Open failed", str(exc))
                    return

                self.mark_dirty()
                self._update_field_preview(field)

                if info is not None:
                    info.config(
                        text=inspect_file_reference(
                            current,
                            field.name,
                            container.DESCRIPTOR.full_name,
                            self.base_dir
                        )
                    )

                self._open_contextual_string(field, current)

            ttk.Button(actions, text="Browse...", command=browse).pack(side=tk.LEFT, padx=2)
            ttk.Button(actions, text="Open", command=open_reference).pack(side=tk.LEFT, padx=2)

            info.config(
                text=inspect_file_reference(
                    value,
                    field.name,
                    container.DESCRIPTOR.full_name,
                    self.base_dir
                )
            )

    def _create_bytes_editor(self, field, container, frame, actions):
        data = bytes(getattr(container, field.name) or b"")

        content = ttk.Frame(frame)
        content.pack(fill=tk.BOTH, expand=True, pady=(0, 4))

        info = ttk.Label(content, text=self._bytes_info(data), anchor=tk.NW, justify=tk.LEFT)
        info.pack(anchor=tk.NW, fill=tk.X)

        path_var = tk.StringVar()

        top = ttk.Frame(content)
        top.pack(fill=tk.X, pady=4)

        ttk.Label(top, text="Load from file:").pack(side=tk.LEFT)
        ttk.Entry(top, textvariable=path_var, width=60).pack(side=tk.LEFT, padx=4)

        def load_bytes(_event=None):
            raw_path = Path(path_var.get()).expanduser()
            if not raw_path.is_absolute():
                raw_path = self.base_dir / raw_path

            if not raw_path.exists():
                messagebox.showerror("Load bytes", f"File not found:\n{raw_path}")
                return

            try:
                payload = raw_path.read_bytes()
            except Exception as exc:
                messagebox.showerror("Load bytes", str(exc))
                return

            try:
                setattr(container, field.name, payload)
            except Exception as exc:
                messagebox.showerror("Load bytes", str(exc))
                return

            self.mark_dirty()
            self._update_field_preview(field)
            info.config(text=self._bytes_info(getattr(container, field.name)))

        def save_bytes():
            out_path = filedialog.asksaveasfilename(
                title="Save bytes",
                defaultextension=".bin",
                filetypes=[("All files", "*.*")]
            )
            if not out_path:
                return

            try:
                Path(out_path).write_bytes(getattr(container, field.name))
                messagebox.showinfo("Saved bytes", f"Wrote {out_path}")
            except Exception as exc:
                messagebox.showerror("Save bytes failed", str(exc))

        def clear_bytes():
            if not messagebox.askyesno("Clear bytes", f"Clear bytes field {field.name}?"):
                return

            try:
                container.ClearField(field.name)
            except Exception as exc:
                messagebox.showerror("Clear bytes failed", str(exc))
                return

            self.mark_dirty()
            self._update_field_preview(field)
            info.config(text=self._bytes_info(getattr(container, field.name)))

        ttk.Button(actions, text="Load", command=load_bytes).pack(side=tk.LEFT, padx=2)
        ttk.Button(actions, text="Save...", command=save_bytes).pack(side=tk.LEFT, padx=2)
        ttk.Button(actions, text="Clear", command=clear_bytes).pack(side=tk.LEFT, padx=2)

    def _bytes_info(self, data) -> str:
        data = bytes(data or b"")
        preview = data[:64].hex()
        if len(data) > 64:
            preview += "..."
        return f"{len(data):,} bytes\nHex preview: {preview}"

    # -------------------------------------------------------------------
    # Repeated-field editors
    # -------------------------------------------------------------------

    def _create_repeated_editor(self, field, container):
        if field.type == TYPE_MESSAGE:
            self._create_repeated_message_editor(field, container)
        else:
            self._create_repeated_scalar_editor(field, container)

    # Repeated message editor.
    def _create_repeated_message_editor(self, field, container):
        self.rm_field = field

        frame = ttk.Frame(self.editor_frame)
        frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        buttons = ttk.Frame(frame)
        buttons.pack(side=tk.TOP, fill=tk.X, pady=(0, 4))

        ttk.Button(buttons, text="Add", command=self._rm_add).pack(side=tk.LEFT, padx=2)
        ttk.Button(buttons, text="Remove", command=self._rm_remove).pack(side=tk.LEFT, padx=2)
        ttk.Button(buttons, text="Move Up", command=lambda: self._rm_move(-1)).pack(
            side=tk.LEFT,
            padx=2
        )
        ttk.Button(buttons, text="Move Down", command=lambda: self._rm_move(1)).pack(
            side=tk.LEFT,
            padx=2
        )
        ttk.Button(buttons, text="Open selected", command=self._rm_open).pack(
            side=tk.LEFT,
            padx=2
        )

        self.rm_hint = ttk.Label(
            frame,
            text="No items yet. Click Add to create an embedded message item.",
            anchor=tk.W
        )

        self.rlist = tk.Listbox(frame, height=14)
        self.rlist.pack(fill=tk.BOTH, expand=True)
        self.rlist.bind("<Double-Button-1>", lambda _event: self._on_repeated_message_double_click())

        self._rm_refresh()

    def _rm_refresh(self):
        if self.rm_field is None or self.rlist is None:
            return

        field = self.rm_field
        container = self.current_message()
        values = getattr(container, field.name)

        self.rlist.delete(0, tk.END)

        for i, item in enumerate(values):
            self.rlist.insert(tk.END, f"[{i}] {self._message_preview(item)}")
        if len(values) == 0 and self.rm_hint is not None:
            self.rm_hint.pack(side=tk.TOP, before=self.rlist, fill=tk.X, pady=(0, 2))
        elif self.rm_hint is not None:
            self.rm_hint.forget()

        self._update_field_preview(field)

    def _rm_add(self):
        field = self.rm_field
        container = self.current_message()
        values = getattr(container, field.name)

        try:
            values.add()
        except Exception as exc:
            messagebox.showerror("Add failed", str(exc))
            return

        self.mark_dirty()
        self._rm_refresh()

        count = len(values)
        if count > 0:
            self.rlist.selection_set(count - 1)
            self.rlist.see(count - 1)

    def _rm_remove(self):
        if self.rlist is None:
            return

        sel = self.rlist.curselection()
        if not sel:
            messagebox.showinfo("Remove", "Select an item to remove.")
            return

        idx = sel[0]
        field = self.rm_field
        container = self.current_message()
        values = getattr(container, field.name)

        if not (0 <= idx < len(values)):
            return

        if not messagebox.askyesno("Remove", f"Remove {field.name}[{idx}]?"):
            return

        try:
            del values[idx]
        except Exception as exc:
            messagebox.showerror("Remove failed", str(exc))
            return

        self.mark_dirty()
        self._rm_refresh()

    def _rm_move(self, delta: int):
        if self.rlist is None:
            return

        sel = self.rlist.curselection()
        if not sel:
            return

        idx = sel[0]
        field = self.rm_field
        container = self.current_message()
        values = getattr(container, field.name)

        target = idx + delta
        if not (0 <= target < len(values)):
            return

        try:
            item = values.pop(idx)
            values.insert(target, item)
        except Exception as exc:
            messagebox.showerror("Move failed", str(exc))
            return

        self.mark_dirty()
        self._rm_refresh()
        self.rlist.selection_set(target)
        self.rlist.see(target)

    def _rm_open(self):
        if self.rlist is None:
            return

        sel = self.rlist.curselection()
        if not sel:
            messagebox.showinfo("Open", "Select an item to open.")
            return

        idx = sel[0]
        field = self.rm_field

        try:
            self._push_field(field, idx)
        except Exception as exc:
            messagebox.showerror("Open failed", str(exc))

    # Repeated scalar editor.
    def _create_repeated_scalar_editor(self, field, container):
        self.rs_field = field
        self.rs_values = getattr(container, field.name)
        self.rs_large = len(self.rs_values) > REPEATED_UI_LIMIT

        frame = ttk.Frame(self.editor_frame)
        frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        buttons = ttk.Frame(frame)
        buttons.pack(side=tk.TOP, fill=tk.X, pady=(0, 4))

        ttk.Button(buttons, text="Add", command=self._rs_add).pack(side=tk.LEFT, padx=2)
        ttk.Button(buttons, text="Remove", command=self._rs_remove).pack(side=tk.LEFT, padx=2)
        ttk.Button(buttons, text="Move Up", command=lambda: self._rs_move(-1)).pack(
            side=tk.LEFT,
            padx=2
        )
        ttk.Button(buttons, text="Move Down", command=lambda: self._rs_move(1)).pack(
            side=tk.LEFT,
            padx=2
        )
        ttk.Button(buttons, text="Import", command=self._rs_import).pack(side=tk.LEFT, padx=2)
        ttk.Button(buttons, text="Export", command=self._rs_export).pack(side=tk.LEFT, padx=2)
        ttk.Button(buttons, text="Clear", command=self._rs_clear).pack(side=tk.LEFT, padx=2)
        ttk.Button(buttons, text="Refresh", command=self._rs_refresh).pack(side=tk.LEFT, padx=2)

        index_value_frame = ttk.Frame(frame)
        index_value_frame.pack(side=tk.TOP, fill=tk.X, pady=2)

        ttk.Label(index_value_frame, text="Index:").pack(side=tk.LEFT)
        self.rs_index_var = tk.StringVar(value="0")
        ttk.Entry(index_value_frame, textvariable=self.rs_index_var, width=14).pack(
            side=tk.LEFT,
            padx=2
        )

        ttk.Label(index_value_frame, text="Value:").pack(side=tk.LEFT, padx=(10, 0))
        self.rs_value_var = tk.StringVar()
        value_entry = ttk.Entry(index_value_frame, textvariable=self.rs_value_var, width=50)
        value_entry.pack(side=tk.LEFT, padx=4)
        value_entry.bind("<Return>", lambda _event: self._rs_set())

        ttk.Button(index_value_frame, text="Set", command=self._rs_set).pack(
            side=tk.LEFT,
            padx=2
        )

        if field.type == TYPE_STRING and is_file_like_field(field, container.DESCRIPTOR.full_name):
            ttk.Button(
                index_value_frame,
                text="Browse file...",
                command=self._rs_browse
            ).pack(side=tk.LEFT, padx=2)

        self.rs_file_info = ttk.Label(
            frame,
            text="",
            anchor=tk.W,
            justify=tk.LEFT,
            wraplength=760
        )
        self.rs_file_info.pack(fill=tk.X, pady=2)

        self.rs_hint = ttk.Label(
            frame,
            text="No values yet. Click Add or Import to add values.",
            anchor=tk.W
        )

        if self.rs_large:
            self.rs_summary = ttk.Label(
                frame,
                text="",
                anchor=tk.NW,
                justify=tk.LEFT,
                font=("Courier New", 9),
                wraplength=760
            )
            self.rs_summary.pack(fill=tk.BOTH, expand=True)
        else:
            self.rs_list = tk.Listbox(frame, height=14)
            self.rs_list.pack(fill=tk.BOTH, expand=True)
            self.rs_list.bind("<<ListboxSelect>>", lambda _event: self._rs_list_select())
            self.rs_list.bind(
                "<Double-Button-1>",
                lambda _event: self._on_repeated_scalar_double_click()
            )

        self._rs_refresh()

    def scan_file_dependencies(self):
        if self.root_msg is None:
            return

        win = tk.Toplevel(self)
        win.configure(bg=THEME["bg"])
        win.title("File Dependencies")
        win.geometry("1000x700")

        text = tk.Text(win, wrap=tk.WORD)
        scroll = ttk.Scrollbar(win, orient="vertical", command=text.yview)
        text.configure(yscrollcommand=scroll.set)

        text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)

        text.tag_configure("missing", foreground=THEME["error"])
        text.tag_configure("warning", foreground=THEME["warn"])
        text.tag_configure("ok", foreground=THEME["accent2"])
        text.tag_configure("note", foreground=THEME["muted"])

        def add(line, tag=None):
            if tag:
                text.insert(tk.END, line + "\n", tag)
            else:
                text.insert(tk.END, line + "\n")

        add("=== File references ===")
        refs = list(self._iter_file_references(self.root_msg, []))

        if not refs:
            add("No non-empty file-like string fields found.", "note")

        for path_str, value in refs:
            status = self._reference_status(value)

            if status.startswith("OK"):
                tag = "ok"
            elif status.startswith("MISSING"):
                tag = "missing"
            else:
                tag = "note"

            add(f"[{status}] {path_str} = {value}", tag)

            detail = inspect_file_reference(value, "unknown", "", self.base_dir)
            for detail_line in detail.splitlines():
                add("    " + detail_line, tag)

            add("")

        add("=== System children ===")
        child_lines = self._system_child_dependency_lines(self.root_msg)

        if not child_lines:
            add("No System.children fields found.", "note")

        for line in child_lines:
            if line.startswith("MISSING"):
                add(line, "missing")
            elif line.startswith("OK"):
                add(line, "ok")
            else:
                add(line, "note")

        add("")

        add("=== Required / recommended fields ===")
        req_lines = self._required_recommended_lines(self.root_msg)

        if not req_lines:
            add("No obvious missing required or recommended fields found.", "note")

        for line in req_lines:
            if line.startswith("MISSING"):
                add(line, "missing")
            elif line.startswith("WARNING"):
                add(line, "warning")
            elif line.startswith("NOTE"):
                add(line, "note")
            elif line.startswith("OK"):
                add(line, "ok")
            else:
                add(line)

        text.config(state=tk.DISABLED)

    def scan_file_references(self):
        """Compatibility wrapper for the comprehensive dependency scanner."""
        self.scan_file_dependencies()

    def _reference_status(self, value):
        value = str(value or "").strip()

        if not value:
            return "EMPTY"

        if "://" in value:
            parsed = urlparse(value)

            if parsed.scheme.lower() == "file":
                path = Path(parsed.path).expanduser()
                return "OK" if path.exists() else "MISSING"

            return "URL (not checked locally)"

        path = self._local_path_for_reference(value)

        if path is None:
            return "URL (not checked locally)"

        return "OK" if path.exists() else "MISSING"

    def _iter_messages_of_type(self, msg, full_name, depth=0):
        if msg.DESCRIPTOR.full_name == full_name:
            yield msg

        if depth > 12:
            return

        for field in msg.DESCRIPTOR.fields:
            if field.type != TYPE_MESSAGE:
                continue

            if field.label == LABEL_REPEATED:
                values = getattr(msg, field.name)
                for i in range(min(len(values), 1000)):
                    yield from self._iter_messages_of_type(values[i], full_name, depth + 1)
            else:
                yield from self._iter_messages_of_type(
                    getattr(msg, field.name),
                    full_name,
                    depth + 1
                )

    def _system_child_dependency_lines(self, root_msg):
        lines = []

        for system in self._iter_messages_of_type(root_msg, "Messages.System"):
            system_name = str(system.meta.name or "unnamed system").strip()

            if not system.children:
                continue

            for child in system.children:
                child = str(child or "").strip()

                if not child:
                    lines.append(f"WARNING empty child name in system '{system_name}'")
                    continue

                path = self._resolve_child_system_path(child, system)

                if path is None:
                    lines.append(
                        f"MISSING child system in '{system_name}': {child} "
                        "(expected .xcs or .json)"
                    )
                else:
                    lines.append(f"OK child system in '{system_name}': {child} = {path}")

        return lines

    def _required_recommended_lines(self, root_msg):
        lines = []

        def empty_message(m):
            try:
                return len(m.ListFields()) == 0
            except Exception:
                return True

        for system in self._iter_messages_of_type(root_msg, "Messages.System"):
            name = str(system.meta.name or "unnamed system").strip()

            if not system.meta.name:
                lines.append("WARNING System.meta.name is empty (recommended)")

            if not system.script:
                lines.append(f"WARNING System '{name}' has no scripts")

            if not system.models:
                lines.append(f"WARNING System '{name}' has no models")

            if not system.referenced_files:
                lines.append(
                    f"NOTE System '{name}' has no referenced files "
                    "(often .xcg geometry)"
                )

            if not system.meta.directory:
                lines.append(
                    f"NOTE System '{name}' has no meta.directory set; "
                    "relative file references use the editor base directory"
                )

        for model in self._iter_messages_of_type(root_msg, "Messages.Model"):
            name = str(model.name or "unnamed model").strip()

            if not model.name:
                lines.append("WARNING Model.name is empty")

            if not model.algorithms:
                lines.append(f"WARNING Model '{name}' has no algorithms")

            if not model.available_outputs:
                lines.append(f"NOTE Model '{name}' has no available_outputs")

        for script in self._iter_messages_of_type(root_msg, "Messages.Script"):
            name = str(script.name or "unnamed script").strip()

            if not script.name:
                lines.append("WARNING Script.name is empty")

            if not script.source_code:
                lines.append(f"WARNING Script '{name}' has no source_code")

            if not script.sequence:
                lines.append(f"NOTE Script '{name}' has no command sequence")

        for function in self._iter_messages_of_type(root_msg, "Messages.Function"):
            if not function.name:
                lines.append("WARNING Function.name is empty")

        for command in self._iter_messages_of_type(root_msg, "Messages.Command"):
            if not command.function.name:
                lines.append("WARNING Command.function.name is empty")
            elif not command.args_are_set:
                lines.append(
                    f"NOTE Command '{command.function.name}' has args_are_set=false"
                )

        for topology in self._iter_messages_of_type(root_msg, "Messages.Topology"):
            name = str(topology.name or "unnamed topology").strip()

            if not topology.name:
                lines.append("WARNING Topology.name is empty")

            if (
                not topology.filename
                and empty_message(topology.nodes)
                and empty_message(topology.edges)
                and empty_message(topology.faces)
                and empty_message(topology.cells)
            ):
                lines.append(
                    f"WARNING Topology '{name}' has no filename and no element data"
                )

        for file_msg in self._iter_messages_of_type(root_msg, "Messages.File"):
            if not file_msg.name:
                lines.append("WARNING File.name is empty")

            if file_msg.is_local and not file_msg.payload:
                lines.append(
                    f"WARNING local File '{file_msg.name or 'unnamed'}' has empty payload"
                )

        return lines

    def _rs_index(self) -> int:
        if self.rs_index_var is None:
            return 0

        try:
            value = int(self.rs_index_var.get())
        except Exception:
            return 0
        return max(0, value)

    def _rs_current_value_text(self) -> str:
        if self.rs_values is None:
            return ""

        idx = self._rs_index()

        if 0 <= idx < len(self.rs_values):
            return str(self.rs_values[idx])

        return ""

    def _rs_refresh(self):
        if self.rs_field is None or self.rs_values is None:
            return

        field = self.rs_field
        values = self.rs_values
        self.rs_values = values

        self.rs_large = len(values) > REPEATED_UI_LIMIT

        if self.rs_large:
            if self.rs_summary is None:
                self._show_field_editor(field)
                return

            lines = [
                f"Large repeated field: {field.name}",
                f"length: {len(values)}",
                f"type: {field_type_text(field)}",
            ]

            if len(values) > 0:
                lines.append(f"first: {short_repr(values[0])}")
                lines.append(f"last: {short_repr(values[-1])}")

                if field.type in NUMERIC_TYPES:
                    sample = values[: min(len(values), 100_000)]
                    try:
                        lines.append(f"min(sample): {min(sample)}")
                        lines.append(f"max(sample): {max(sample)}")
                    except Exception:
                        pass

            self.rs_summary.config(text="\n".join(lines))
        else:
            if self.rs_list is None:
                self._show_field_editor(field)
                return

            self.rs_list.delete(0, tk.END)
            for i in range(min(len(values), REPEATED_UI_LIMIT)):
                self.rs_list.insert(tk.END, str(values[i]))

        count = len(values)
        idx = self._rs_index()

        if self.rs_index_var is not None:
            if count == 0:
                self.rs_index_var.set("0")
            elif idx >= count:
                self.rs_index_var.set(str(count - 1))

        if self.rs_value_var is not None:
            self.rs_value_var.set(self._rs_current_value_text())

        self._rs_update_file_info()

        target = self.rs_list if self.rs_list is not None else self.rs_summary

        if len(values) == 0 and self.rs_hint is not None and target is not None:
            self.rs_hint.pack(side=tk.TOP, before=target, fill=tk.X, pady=(0, 2))
        elif self.rs_hint is not None:
            self.rs_hint.forget()

        self._update_field_preview(field)

    def _rs_update_file_info(self):
        if self.rs_file_info is None:
            return

        field = self.rs_field
        if field is None or self.rs_values is None:
            self.rs_file_info.config(text="")
            return

        if field.type == TYPE_STRING and is_file_like_field(field, self.current_message().DESCRIPTOR.full_name):
            idx = self._rs_index()
            values = self.rs_values
            value = values[idx] if 0 <= idx < len(values) else ""

            self.rs_file_info.config(
                text=inspect_file_reference(
                    value,
                    field.name,
                    self.current_message().DESCRIPTOR.full_name,
                    self.base_dir,
                )
            )
        else:
            self.rs_file_info.config(text="")

    def _rs_list_select(self):
        if self.rs_list is None or self.rs_values is None:
            return

        sel = self.rs_list.curselection()
        if not sel:
            return

        idx = sel[0]
        if self.rs_index_var is not None:
            self.rs_index_var.set(str(idx))

        if self.rs_value_var is not None and 0 <= idx < len(self.rs_values):
            self.rs_value_var.set(str(self.rs_values[idx]))

        self._rs_update_file_info()

    def _rs_add(self):
        if self.rs_field is None or self.rs_values is None:
            return

        values = self.rs_values

        try:
            values.append(default_scalar_value(self.rs_field))
        except Exception as exc:
            messagebox.showerror("Add failed", str(exc))
            return

        self.mark_dirty()

        if len(values) > REPEATED_UI_LIMIT and not self.rs_large:
            self._show_field_editor(self.rs_field)
        else:
            if self.rs_index_var is not None:
                self.rs_index_var.set(str(len(values) - 1))
            self._rs_refresh()

    def _rs_remove(self):
        if self.rs_values is None:
            return

        idx = self._rs_index()
        values = self.rs_values

        if not (0 <= idx < len(values)):
            messagebox.showinfo("Remove", "Select or set a valid index to remove.")
            return

        if not messagebox.askyesno("Remove", f"Remove index {idx}?"):
            return

        try:
            del values[idx]
        except Exception as exc:
            messagebox.showerror("Remove failed", str(exc))
            return

        self.mark_dirty()

        if len(values) <= REPEATED_UI_LIMIT and self.rs_large:
            self._show_field_editor(self.rs_field)
        else:
            self._rs_refresh()

    def _rs_move(self, delta: int):
        if self.rs_values is None:
            return

        idx = self._rs_index()
        target = idx + delta
        values = self.rs_values

        if not (0 <= idx < len(values) and 0 <= target < len(values)):
            return

        try:
            item = values.pop(idx)
            values.insert(target, item)
        except Exception as exc:
            messagebox.showerror("Move failed", str(exc))
            return

        self.mark_dirty()
        if self.rs_index_var is not None:
            self.rs_index_var.set(str(target))
        self._rs_refresh()

    def _rs_set(self):
        if self.rs_field is None or self.rs_values is None or self.rs_value_var is None:
            return

        idx = self._rs_index()
        values = self.rs_values

        try:
            value = coerce_value(self.rs_field, self.rs_value_var.get())
        except Exception as exc:
            messagebox.showerror("Invalid value", str(exc))
            return

        if idx < 0 or idx > len(values):
            messagebox.showerror("Set failed", f"Index {idx} is out of range.")
            return

        try:
            if idx == len(values):
                values.append(value)
            else:
                values[idx] = value
        except Exception as exc:
            messagebox.showerror("Set failed", str(exc))
            return

        self.mark_dirty()
        if self.rs_index_var is not None:
            self.rs_index_var.set(str(idx))
        self._rs_refresh()

    def _rs_clear(self):
        if self.rs_values is None:
            return

        if not messagebox.askyesno("Clear", "Clear all values in this repeated field?"):
            return

        try:
            self.rs_values.clear()
        except Exception as exc:
            messagebox.showerror("Clear failed", str(exc))
            return

        self.mark_dirty()
        self._show_field_editor(self.rs_field)

    def _rs_import(self):
        if self.rs_field is None or self.rs_values is None:
            return

        path = filedialog.askopenfilename(
            title="Import repeated values",
            filetypes=[("Text/CSV", "*.txt *.csv"), ("All files", "*.*")],
        )
        if not path:
            return

        try:
            text = Path(path).read_text(encoding="utf-8", errors="replace")
        except Exception as exc:
            messagebox.showerror("Import failed", str(exc))
            return

        lines = [line.strip() for line in text.splitlines() if line.strip()]
        if not lines:
            messagebox.showinfo("Import", "No values found.")
            return

        tokens = lines

        # If the first line contains commas and the field is numeric,
        # allow CSV-style import.
        if self.rs_field.type != TYPE_STRING and "," in lines[0]:
            tokens = text.replace(",", " ").split()

        parsed = []
        for token in tokens:
            try:
                parsed.append(coerce_value(self.rs_field, token))
            except Exception as exc:
                messagebox.showerror("Import failed", f"Bad token {token!r}:\n{exc}")
                return

        try:
            self.rs_values.clear()
            self.rs_values.extend(parsed)
        except Exception as exc:
            messagebox.showerror("Import failed", str(exc))
            return

        self.mark_dirty()
        self._show_field_editor(self.rs_field)

    def _rs_export(self):
        if self.rs_values is None:
            return

        path = filedialog.asksaveasfilename(
            title="Export repeated values",
            defaultextension=".txt",
            filetypes=[("Text", "*.txt"), ("All files", "*.*")],
        )
        if not path:
            return

        try:
            with open(path, "w", encoding="utf-8") as fout:
                for value in self.rs_values:
                    fout.write(f"{value}\n")
        except Exception as exc:
            messagebox.showerror("Export failed", str(exc))
            return

        messagebox.showinfo("Export", f"Wrote {path}")

    def _rs_browse(self):
        if self.rs_value_var is None or self.rs_field is None:
            return

        path = filedialog.askopenfilename(title="Select file")
        if not path:
            return

        self.rs_value_var.set(str(path))
        self._rs_set()

    # -------------------------------------------------------------------
    # File reference scanning
    # -------------------------------------------------------------------

    def _iter_file_references(self, msg, parts, depth: int = 0):
        if depth > 12:
            return

        for field in msg.DESCRIPTOR.fields:
            name = field.name

            if field.label == LABEL_REPEATED:
                values = getattr(msg, field.name)
                limit = 1000

                for i in range(min(len(values), limit)):
                    item = values[i]
                    child_parts = parts + [f"{name}[{i}]"]

                    if field.type == TYPE_MESSAGE:
                        yield from self._iter_file_references(item, child_parts, depth + 1)
                    elif field.type == TYPE_STRING and is_file_like_field(
                        field,
                        msg.DESCRIPTOR.full_name,
                    ):
                        value = str(item)
                        if value.strip():
                            yield (" | ".join(child_parts), value)

                # Optional: if a repeated string field is file-like and very large,
                # we intentionally stop scanning at 1000 items to avoid UI flooding.

            else:
                if field.type == TYPE_MESSAGE:
                    yield from self._iter_file_references(
                        getattr(msg, field.name),
                        parts + [name],
                        depth + 1,
                    )
                elif field.type == TYPE_STRING and is_file_like_field(
                    field,
                    msg.DESCRIPTOR.full_name,
                ):
                    value = str(getattr(msg, field.name) or "")
                    if value.strip():
                        yield (" | ".join(parts + [name]), value)

    # -------------------------------------------------------------------
    # Status and title
    # -------------------------------------------------------------------

    def mark_dirty(self):
        self.dirty = True
        self._update_status()
        self._update_title()

    def _update_status(self):
        loc = str(self.load_path) if self.load_path is not None else "no file"
        fmt = self.input_format if self.input_format is not None else "new"
        dirty = " modified" if self.dirty else ""
        self.status_var.set(f"{loc}  |  format={fmt}{dirty}")

    def _update_title(self):
        if self.load_path is not None:
            title = f"XCMessages Editor - {self.load_path.name}"
        else:
            title = f"XCMessages Editor - new {self.root_cls.__name__}"

        if self.dirty:
            title = "*" + title

        self.title(title)

    def _on_close(self):
        if self.dirty and not messagebox.askyesno(
            "Exit",
            "Discard unsaved changes and exit?",
        ):
            return

        self.destroy()


# -----------------------------------------------------------------------------
# Entry point.
# -----------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Graphical editor for Xplicit Computing MESSAGES protobuf files."
    )
    parser.add_argument("file", nargs="?", help="Optional input .xcs/.xcg/.xcm/.xco/.pb/.json file.")
    parser.add_argument(
        "-t",
        "--type",
        default="System",
        help="Root protobuf message type to use for new files and ambiguous opens. Default: System.",
    )
    args = parser.parse_args()

    app = XCMessagesEditor(initial_type=args.type, initial_file=args.file)
    app.mainloop()


if __name__ == "__main__":
    main()

