#!/usr/bin/env python3
import argparse
import os
import shlex
import subprocess
import sys
import threading
import traceback

# Setup i18n first, before importing Gtk
try:
    from infinisim_launcher_i18n import CURRENT_LANG
except ImportError:
    CURRENT_LANG = "es"

try:
    import gi

    gi.require_version("Gtk", "3.0")
    from gi.repository import GLib, Gtk, Gdk
    import cairo
    import math
except Exception:
    # 90 means: launcher should fall back to text/zenity mode.
    sys.exit(90)


class ActionRow(Gtk.ListBoxRow):
    def __init__(self, action, icon_name, title, description):
        super().__init__()
        self.action = action
        self.get_style_context().add_class("action-row")

        outer = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=14)
        outer.set_margin_top(12)
        outer.set_margin_bottom(12)
        outer.set_margin_start(14)
        outer.set_margin_end(14)

        icon_halo = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        icon_halo.get_style_context().add_class("action-icon-halo")
        icon_halo.set_size_request(44, 44)

        icon = Gtk.Image.new_from_icon_name(icon_name, Gtk.IconSize.DIALOG)
        icon.set_pixel_size(22)
        icon.set_valign(Gtk.Align.CENTER)
        icon_halo.pack_start(icon, True, True, 0)

        text_col = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=3)
        text_col.set_hexpand(True)

        title_label = Gtk.Label(label=title)
        title_label.set_xalign(0)
        title_label.get_style_context().add_class("action-title")
        title_label.set_line_wrap(True)

        description_label = Gtk.Label(label=description)
        description_label.set_xalign(0)
        description_label.set_line_wrap(True)
        description_label.set_max_width_chars(62)
        description_label.get_style_context().add_class("action-subtitle")

        chevron = Gtk.Image.new_from_icon_name(
            "go-next-symbolic", Gtk.IconSize.MENU
        )
        chevron.get_style_context().add_class("action-chevron")
        chevron.set_halign(Gtk.Align.END)
        chevron.set_valign(Gtk.Align.CENTER)

        text_col.pack_start(title_label, False, False, 0)
        text_col.pack_start(description_label, False, False, 0)

        outer.pack_start(icon_halo, False, False, 0)
        outer.pack_start(text_col, True, True, 0)
        outer.pack_start(chevron, False, False, 0)
        self.add(outer)


class ProgressIndicator(Gtk.Box):
    """Visual progress indicator with elegant gradient circles. Current step is slightly larger."""
    
    def __init__(self, total_steps):
        super().__init__(orientation=Gtk.Orientation.HORIZONTAL, spacing=12)
        self.set_halign(Gtk.Align.CENTER)
        self.set_valign(Gtk.Align.CENTER)
        
        self.total_steps = total_steps
        self.current_step = 0
        self.circles = []
        
        # Darker, more elegant colors
        self.color_active = (0.18, 0.40, 0.75)      # #2d66be - Dark blue
        self.color_inactive = (0.75, 0.75, 0.75)     # #bfbfbf - Light gray
        
        for i in range(total_steps):
            circle = Gtk.DrawingArea()
            circle.set_size_request(20, 20)  # Smaller, more elegant
            circle.connect("draw", self._on_draw_circle, i)
            self.circles.append(circle)
            self.pack_start(circle, False, False, 0)
    
    def set_current_step(self, step):
        """Update which step is active (0-indexed)."""
        if 0 <= step < self.total_steps:
            self.current_step = step
            for circle in self.circles:
                circle.queue_draw()
    
    def _on_draw_circle(self, widget, cr, step_index):
        """Draw an elegant gradient circle with radial fade."""
        width = widget.get_allocated_width()
        height = widget.get_allocated_height()
        center_x = width / 2.0
        center_y = height / 2.0
        
        if step_index == self.current_step:
            # Current step: slightly larger, with radial gradient
            radius = 6.5
            color = self.color_active
        else:
            # Other steps: smaller, subtle
            radius = 5
            color = self.color_inactive
        
        # Create radial gradient for soft, elegant fade
        gradient = cairo.RadialGradient(center_x, center_y, 0, center_x, center_y, radius)
        
        # Gradient: center bright, fade to transparent at edges
        gradient.add_color_stop_rgba(0, color[0], color[1], color[2], 0.9)  # Center: opaque
        gradient.add_color_stop_rgba(1, color[0], color[1], color[2], 0.2)  # Edge: transparent
        
        # Draw the gradient circle
        cr.set_source(gradient)
        cr.arc(center_x, center_y, radius, 0, 2 * math.pi)
        cr.fill()
        
        # Subtle outer ring for definition
        cr.arc(center_x, center_y, radius, 0, 2 * math.pi)
        cr.set_source_rgba(color[0] * 0.6, color[1] * 0.6, color[2] * 0.6, 0.3)
        cr.set_line_width(0.5)
        cr.stroke()
        
        return False


class ProgressDialog(Gtk.Dialog):
    """Floating modal dialog for build progress, styled like Elementary OS assistant."""
    
    def __init__(self, parent, launcher_window):
        super().__init__(type=Gtk.WindowType.TOPLEVEL)
        self.launcher_window = launcher_window
        self.set_transient_for(parent)
        self.set_modal(True)
        self.set_decorated(False)
        self.set_keep_above(True)
        self.set_position(Gtk.WindowPosition.CENTER_ON_PARENT)
        
        # Style class for modal
        self.get_style_context().add_class("progress-modal")
        
        # Get content area but don't use default buttons
        content = self.get_content_area()
        content.set_spacing(0)
        content.set_margin_top(0)
        content.set_margin_bottom(0)
        content.set_margin_start(0)
        content.set_margin_end(0)
        
        # Main vertical container
        main_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        main_box.get_style_context().add_class("app-shell")
        
        # Content area with padding
        content_area = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=14)
        content_area.set_margin_top(24)
        content_area.set_margin_bottom(24)
        content_area.set_margin_start(22)
        content_area.set_margin_end(22)
        
        # Status header (spinner + title + subtitle)
        top = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=14)
        top.set_hexpand(True)
        
        self.spinner = Gtk.Spinner()
        self.spinner.set_size_request(36, 36)
        
        text_col = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=2)
        self.status_title = Gtk.Label(label=_("Preparing..."))
        self.status_title.set_xalign(0)
        self.status_title.get_style_context().add_class("inline-heading")
        
        self.status_subtitle = Gtk.Label(label=_("Initializing process"))
        self.status_subtitle.set_xalign(0)
        self.status_subtitle.get_style_context().add_class("inline-subtitle")
        
        text_col.pack_start(self.status_title, False, False, 0)
        text_col.pack_start(self.status_subtitle, False, False, 0)
        
        top.pack_start(self.spinner, False, False, 0)
        top.pack_start(text_col, True, True, 0)
        
        content_area.pack_start(top, False, False, 0)
        
        # Log details expander
        details_frame = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=8)
        details_frame.get_style_context().add_class("details-box")
        
        self.details_expander = Gtk.Expander.new(_("Execution Details"))
        self.details_expander.set_expanded(False)
        
        scrolled = Gtk.ScrolledWindow()
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled.set_size_request(-1, 180)
        scrolled.get_style_context().add_class("log-surface")
        
        self.log_view = Gtk.TextView()
        self.log_view.set_editable(False)
        self.log_view.set_monospace(True)
        self.log_view.set_cursor_visible(False)
        self.log_view.get_style_context().add_class("log-text")
        
        scrolled.add(self.log_view)
        self.details_expander.add(scrolled)
        details_frame.pack_start(self.details_expander, False, False, 0)
        
        content_area.pack_start(details_frame, True, True, 4)
        
        # Breadcrumbs + Action buttons row
        action_row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=12)
        action_row.set_hexpand(True)
        
        self.cancel_button = Gtk.Button.new_with_label(_("Cancel"))
        self.cancel_button.get_style_context().add_class("danger-button")
        self.cancel_button.get_style_context().add_class("destructive-action")
        self.cancel_button.set_size_request(160, -1)
        
        # Progress indicator with 3 steps: Select -> Build -> Ready
        self.progress_indicator = ProgressIndicator(3)
        self.progress_indicator.set_current_step(0)  # Start at "Seleccionar"
        
        self.start_button = Gtk.Button.new_with_label(_("Start Simulator"))
        self.start_button.get_style_context().add_class("primary-button")
        self.start_button.get_style_context().add_class("suggested-action")
        self.start_button.set_size_request(160, -1)
        
        action_row.pack_start(self.cancel_button, False, False, 0)
        action_row.pack_start(Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL), True, True, 0)
        action_row.pack_start(self.progress_indicator, False, False, 0)
        action_row.pack_start(Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL), True, True, 0)
        action_row.pack_end(self.start_button, False, False, 0)
        
        content_area.pack_start(action_row, False, False, 8)
        
        main_box.pack_start(content_area, True, True, 0)
        content.pack_start(main_box, True, True, 0)
        
        # Set size to content + some padding
        self.set_size_request(760, 420)
        
        self.show_all()


class LauncherWindow(Gtk.ApplicationWindow):
    def __init__(self, app, backend_path, simulator_args):
        super().__init__(application=app)
        self.backend_path = backend_path
        self.simulator_args = simulator_args
        self.process = None
        self.is_running = False
        self.cancel_requested = False
        self.last_error_line = ""
        self.error_type = ""
        self.current_action = ""
        self.ready_binary_path = ""
        self.progress_dialog = None

        self.set_title("InfiniSim")
        self.set_default_size(920, 620)
        self.set_position(Gtk.WindowPosition.CENTER)

        self.connect("key-press-event", self._on_key_press)
        self._setup_css()

        root = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        root.get_style_context().add_class("window-root")
        self.add(root)

        header = Gtk.HeaderBar()
        header.set_show_close_button(True)
        header.set_title("InfiniSim")
        header.set_subtitle(_("Launcher for the simulator"))
        self.set_titlebar(header)

        self.choice_page = self._build_choice_page()
        root.pack_start(self.choice_page, True, True, 0)

    def _setup_css(self):
        provider = Gtk.CssProvider()
        provider.load_from_data(
            b"""
            .window-root {
                background: linear-gradient(180deg, #f7f9fc 0%, #edf1f6 100%);
            }

            .eyebrow {
                color: #5f6b7d;
                font-size: 12px;
                font-weight: 600;
                letter-spacing: 0.4px;
            }

            .hero-title {
                color: #1f2735;
                font-size: 38px;
                font-weight: 800;
            }

            .hero-subtitle {
                color: #4f5b6e;
                font-size: 17px;
                font-weight: 700;
            }

            .logo-wrap {
                background: #ffffff;
                border-radius: 999px;
                border: 1px solid #d8e0ea;
                box-shadow: 0 6px 20px rgba(25, 35, 56, 0.12);
            }

            .choice-card {
                background: rgba(255, 255, 255, 0.78);
                border-radius: 12px;
                border: 1px solid #d7dee8;
            }

            .app-shell {
                background: rgba(255, 255, 255, 0.95);
                border-radius: 12px;
                border: 1px solid #d7dee8;
            }

            .progress-modal {
                background: #ffffff;
            }

            .inline-heading {
                font-size: 24px;
                font-weight: 700;
                color: #1f2933;
            }

            .inline-subtitle {
                color: #556070;
                font-size: 14px;
            }

            .action-row {
                background: transparent;
                border-left: 4px solid transparent;
                border-radius: 10px;
            }

            .action-row:selected {
                background: #edf3ff;
                border-left: 4px solid #3584e4;
            }

            .action-row image,
            .action-row:selected image {
                color: #2f435f;
                -gtk-icon-effect: none;
            }

            .action-icon-halo {
                background: #f2f5fa;
                border-radius: 999px;
                border: 1px solid #dde4ee;
            }

            .action-row:selected .action-icon-halo {
                background: #dfe9ff;
                border: 1px solid #b8ccf4;
            }

            .action-title {
                color: #1f2933;
                font-size: 21px;
                font-weight: 700;
            }

            .action-subtitle {
                color: #5b6473;
                font-size: 13px;
            }

            .action-chevron {
                color: #7f8ba0;
            }

            .primary-button {
                border-radius: 10px;
                padding: 10px 16px;
                font-weight: 600;
            }

            .danger-button {
                border-radius: 10px;
                padding: 10px 16px;
                font-weight: 600;
            }

            .details-box {
                border-top: 1px solid #d8dfe8;
                padding-top: 8px;
            }

            .log-surface {
                border: 1px solid #282c34;
                border-radius: 10px;
                background: #1c1f26;
            }

            .log-text {
                color: #a1efe4;
                background: #1c1f26;
            }
            """
        )
        Gtk.StyleContext.add_provider_for_screen(
            self.get_screen(), provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
        )

    def _build_choice_page(self):
        outer = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        outer.set_margin_top(34)
        outer.set_margin_bottom(34)
        outer.set_margin_start(34)
        outer.set_margin_end(34)

        content = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=16)
        content.set_halign(Gtk.Align.CENTER)
        content.set_valign(Gtk.Align.CENTER)
        content.set_vexpand(True)
        content.set_hexpand(True)
        content.set_size_request(760, -1)

        logo_wrap = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        logo_wrap.set_size_request(84, 84)
        logo_wrap.set_halign(Gtk.Align.CENTER)
        logo_wrap.get_style_context().add_class("logo-wrap")

        logo = Gtk.Image()
        logo_path = self._resolve_logo_path()
        if logo_path:
            logo.set_from_file(logo_path)
        else:
            logo.set_from_icon_name("applications-development", Gtk.IconSize.DIALOG)
        logo.set_pixel_size(44)
        logo_wrap.pack_start(logo, True, True, 0)

        heading = Gtk.Label(label=_("Emulate InfiniTime"))
        heading.set_xalign(0.5)
        heading.get_style_context().add_class("hero-title")

        subtitle = Gtk.Label(
            label=_("Select a path to open or compile the simulator in a few steps.")
        )
        subtitle.set_xalign(0.5)
        subtitle.set_justify(Gtk.Justification.CENTER)
        subtitle.set_line_wrap(True)
        subtitle.set_max_width_chars(56)
        subtitle.get_style_context().add_class("hero-subtitle")

        card = Gtk.Frame()
        card.get_style_context().add_class("choice-card")
        card.set_size_request(720, -1)

        self.listbox = Gtk.ListBox()
        self.listbox.set_activate_on_single_click(True)
        self.listbox.set_selection_mode(Gtk.SelectionMode.SINGLE)
        self.listbox.connect("row-activated", self._on_action_activated)

        self.listbox.add(
            ActionRow(
                "build-local",
                "folder-open-symbolic",
                _("Select local folder to compile"),
                _("Compile using your local InfiniTime folder."),
            )
        )
        self.listbox.add(
            ActionRow(
                "build-clone",
                "folder-publicshare-symbolic",
                _("Download official repository and compile"),
                _("Download the official repo and compile automatically."),
            )
        )

        card.add(self.listbox)

        content.pack_start(logo_wrap, False, False, 0)
        content.pack_start(heading, False, False, 0)
        content.pack_start(subtitle, False, False, 0)
        content.pack_start(card, True, True, 4)
        outer.pack_start(content, True, True, 0)
        return outer

    def _resolve_logo_path(self):
        appdir = os.environ.get("APPDIR", "")
        candidates = []

        if appdir:
            candidates.append(
                os.path.join(
                    appdir,
                    "usr",
                    "share",
                    "icons",
                    "hicolor",
                    "scalable",
                    "apps",
                    "infinisim.svg",
                )
            )

        script_dir = os.path.dirname(os.path.abspath(__file__))
        candidates.append(
            os.path.join(script_dir, "..", "packaging", "appimage", "infinisim.svg")
        )

        for path in candidates:
            normalized = os.path.abspath(path)
            if os.path.isfile(normalized):
                return normalized

        return None

    def _on_action_activated(self, _listbox, row):
        action = row.action
        infinitime_dir = None

        if action == "build-local":
            infinitime_dir = self._choose_directory(
                _("Select the local InfiniTime folder")
            )
            if not infinitime_dir:
                return

        # Open progress dialog
        self.progress_dialog = ProgressDialog(self, self)
        self.progress_dialog.cancel_button.connect("clicked", self._on_cancel_clicked)
        self.progress_dialog.start_button.connect("clicked", self._on_start_clicked)
        
        self._start_backend(action, infinitime_dir=infinitime_dir)

    def _choose_directory(self, title):
        dialog = Gtk.FileChooserDialog(
            title=title,
            parent=self,
            action=Gtk.FileChooserAction.SELECT_FOLDER,
        )
        dialog.add_buttons(
            Gtk.STOCK_CANCEL,
            Gtk.ResponseType.CANCEL,
            _("Select"),
            Gtk.ResponseType.OK,
        )

        result = None
        response = dialog.run()
        if response == Gtk.ResponseType.OK:
            result = dialog.get_filename()
        dialog.destroy()
        return result

    def _append_log(self, line):
        if not self.progress_dialog:
            return
            
        marker_prefix = "[INFINISIM_READY_BINARY:"
        if line.startswith(marker_prefix) and line.rstrip().endswith("]"):
            self.ready_binary_path = line.strip()[len(marker_prefix):-1]
            return

        lower_line = line.lower()
        if "error:" in lower_line and "info:" not in lower_line:
            self.last_error_line = line.strip()
            if "[infinisim_error_type:" in lower_line:
                start = lower_line.find("[infinisim_error_type:")
                end = lower_line.find("]", start)
                if end > start:
                    self.error_type = lower_line[start + 21:end]

        # Mirror launcher output to stderr so terminal users can inspect logs.
        try:
            sys.stderr.write(line)
            if not line.endswith("\n"):
                sys.stderr.write("\n")
            sys.stderr.flush()
        except Exception:
            pass

        buffer_obj = self.progress_dialog.log_view.get_buffer()
        end_iter = buffer_obj.get_end_iter()
        buffer_obj.insert(end_iter, line)
        mark = buffer_obj.create_mark(None, buffer_obj.get_end_iter(), False)
        self.progress_dialog.log_view.scroll_to_mark(mark, 0.0, True, 0.0, 1.0)

    def _start_backend(self, action, binary=None, infinitime_dir=None):
        if not self.progress_dialog:
            return
            
        self.is_running = True
        self.cancel_requested = False
        self.last_error_line = ""
        self.error_type = ""
        self.current_action = action
        self.ready_binary_path = ""
        self.process = None

        self.progress_dialog.log_view.get_buffer().set_text("")
        self.progress_dialog.status_title.set_text(_("⚙️ Preparing simulator"))
        self.progress_dialog.status_subtitle.set_text(_("Starting build/startup tasks"))
        self.progress_dialog.progress_indicator.set_current_step(1)  # Compilando
        self.progress_dialog.details_expander.set_expanded(False)
        self.progress_dialog.cancel_button.set_sensitive(True)
        self.progress_dialog.cancel_button.show()
        self.progress_dialog.start_button.set_sensitive(False)
        self.progress_dialog.spinner.start()

        command = [
            self.backend_path,
            "--launcher-no-gui",
            "--launcher-detach",
            "--launcher-action",
            action,
        ]

        if binary:
            command.extend(["--launcher-binary", binary])
        if infinitime_dir:
            command.extend(["--launcher-infinitime-dir", infinitime_dir])

        runtime_args = list(self.simulator_args)
        command.append("--")
        command.extend(runtime_args)

        self._append_log("$ " + " ".join(shlex.quote(part) for part in command) + "\n")

        env = os.environ.copy()
        env["INFINISIM_DETACH"] = "1"
        env["INFINISIM_EMBEDDED_UI"] = "1"
        if action in ("build-local", "build-clone"):
            env["INFINISIM_BUILD_ONLY"] = "1"

        try:
            self.process = subprocess.Popen(
                command,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1,
                env=env,
            )
        except Exception as exc:
            self.progress_dialog.spinner.stop()
            self.is_running = False
            self.progress_dialog.cancel_button.set_sensitive(False)
            self._append_log(_("Error starting backend: {exc}").format(exc=exc) + "\n")
            self.progress_dialog.details_expander.set_expanded(True)
            self._show_error(
                _("Could not start the assistant"),
                _("An error occurred while starting the task. Check the execution details."),
            )
            return

        def read_output():
            try:
                assert self.process is not None
                for line in self.process.stdout:
                    GLib.idle_add(self._append_log, line)
                exit_code = self.process.wait()
                GLib.idle_add(self._on_process_done, exit_code)
            except Exception:
                GLib.idle_add(
                    self._append_log,
                    _("Exception reading output:") + "\n" + traceback.format_exc() + "\n",
                )
                GLib.idle_add(self._on_process_done, 1)

        threading.Thread(target=read_output, daemon=True).start()

    def _on_process_done(self, exit_code):
        if not self.progress_dialog:
            return
            
        self.progress_dialog.spinner.stop()
        self.progress_dialog.cancel_button.set_sensitive(False)
        self.is_running = False

        if self.cancel_requested:
            self.cancel_requested = False
            self._close_progress_dialog()
            return False

        if exit_code == 0:
            if self.current_action in ("build-local", "build-clone") and self.ready_binary_path:
                self.progress_dialog.status_title.set_text(_("✅ Compilation ready"))
                self.progress_dialog.status_subtitle.set_text(
                    _("You can start the simulator or close this window.")
                )
                self.progress_dialog.progress_indicator.set_current_step(2)  # Listo
                self.progress_dialog.cancel_button.set_sensitive(False)
                self.progress_dialog.start_button.set_sensitive(True)
                self._append_log(_("Compilation finished successfully.") + "\n")
            else:
                self.progress_dialog.status_title.set_text(_("Simulator ready"))
                self.progress_dialog.status_subtitle.set_text(
                    _("Started successfully. You can close this window.")
                )
                self._append_log(_("Simulator started successfully.") + "\n")
                GLib.timeout_add(2000, self._close_progress_dialog)
            return False

        # Error handling
        error_messages = {
            "invalid_path": (_("Invalid path"), _("The selected folder does not exist or is not accessible.")),
            "sim_launch": (_("Simulator did not start"), _("The main window could not be opened. Check graphics dependencies.")),
            "cmake_config": (_("Invalid configuration"), _("The InfiniTime folder does not appear to be valid.")),
            "cmake_build": (_("Compilation failed"), _("There were errors compiling the code. Check the details above.")),
        }
        
        title, message = error_messages.get(
            self.error_type, 
            (_("Could not start InfiniSim"), _("An error occurred. Check the details."))
        )

        self._show_error(title, message)
        return False

    def _show_error(self, title, message):
        if not self.progress_dialog:
            return
            
        self._append_log(f"ERROR: {title}\n{message}\n")
        self.is_running = False
        self.progress_dialog.spinner.stop()
        self.progress_dialog.cancel_button.set_sensitive(True)
        self.progress_dialog.cancel_button.set_label(_("Cancel"))
        self.progress_dialog.start_button.set_sensitive(False)
        self.progress_dialog.status_title.set_text("❌ " + title)
        self.progress_dialog.status_subtitle.set_text(message)
        self.progress_dialog.details_expander.set_expanded(True)

    def _on_cancel_clicked(self, button):
        if self.progress_dialog.cancel_button.get_label() == "Cerrar":
            self._close_progress_dialog()
            return
            
        if not self.is_running:
            self._close_progress_dialog()
            return

        self.cancel_requested = True
        self.progress_dialog.cancel_button.set_sensitive(False)
        self.progress_dialog.status_title.set_text(_("Canceling compilation"))
        self.progress_dialog.status_subtitle.set_text(_("Stopping process..."))
        self._append_log(_("Cancellation requested by user.") + "\n")

        try:
            if self.process and self.process.poll() is None:
                self.process.terminate()
        except Exception as exc:
            self._append_log(_("Could not cancel: {exc}").format(exc=exc) + "\n")

    def _on_start_clicked(self, _button):
        if not self.ready_binary_path:
            self._show_error(
                _("Could not start"),
                _("The compiled binary was not found to start the simulator."),
            )
            return
        self._start_backend("run-compiled", binary=self.ready_binary_path)

    def _close_progress_dialog(self):
        """Close the progress dialog and return to main window."""
        if self.progress_dialog:
            self.progress_dialog.destroy()
            self.progress_dialog = None
            # Reset state
            self.is_running = False
            self.cancel_requested = False
            self.ready_binary_path = ""

    def _on_key_press(self, widget, event):
        """Handle Escape key to close the window."""
        if event.keyval == Gdk.KEY_Escape:
            self.close()
            return True
        return False


class LauncherApplication(Gtk.Application):
    def __init__(self, backend_path, simulator_args):
        super().__init__(application_id="org.infinitime.infinisim.launcher")
        self.backend_path = backend_path
        self.simulator_args = simulator_args

    def do_activate(self):
        try:
            window = LauncherWindow(self, self.backend_path, self.simulator_args)
            window.show_all()
        except Exception:
            details = traceback.format_exc()
            print(details, file=sys.stderr)
            print(
                _("InfiniSim could not open the assistant. Check the console output for details."),
                file=sys.stderr,
            )
            self.quit()


def parse_args():
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--backend", required=True)
    parser.add_argument("sim_args", nargs=argparse.REMAINDER)
    args = parser.parse_args()

    sim_args = list(args.sim_args)
    if sim_args and sim_args[0] == "--":
        sim_args = sim_args[1:]

    return args.backend, sim_args


def main():
    backend_path, sim_args = parse_args()
    app = LauncherApplication(backend_path, sim_args)
    # Gtk.Application parses argv again; pass only program name to avoid
    # treating our custom --backend flag as an unknown GTK option.
    return app.run([sys.argv[0]])


if __name__ == "__main__":
    sys.exit(main())
