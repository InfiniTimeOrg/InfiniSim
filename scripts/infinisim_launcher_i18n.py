#!/usr/bin/env python3
"""
i18n module for InfiniSim Launcher UI
Provides gettext-based localization support
"""
import gettext
import locale
import os
import sys

def setup_i18n():
    """Initialize gettext for the launcher UI."""
    # Determine the locale directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    appdir = os.environ.get("APPDIR", "")
    
    locale_dirs = []
    
    # Prefer APPDIR location (AppImage)
    if appdir:
        locale_dirs.append(os.path.join(appdir, "usr", "share", "locale"))
    
    # Fallback to script directory + ../share/locale
    locale_dirs.append(os.path.join(script_dir, "..", "share", "locale"))
    
    # System locale directory
    locale_dirs.append("/usr/share/locale")
    
    # Find the first valid locale directory
    locale_dir = None
    for path in locale_dirs:
        if os.path.isdir(path):
            locale_dir = path
            break
    
    # Get system language, default to Spanish if not supported
    try:
        lang, _ = locale.getdefaultlocale()
        if lang:
            lang = lang.split("_")[0]  # Extract language code (e.g., "es" from "es_ES")
        else:
            lang = "es"
    except Exception:
        lang = "es"
    
    # Ensure we have a supported language fallback
    supported_langs = ["en", "es"]
    if lang not in supported_langs:
        lang = "es"  # Fallback to Spanish
    
    try:
        if locale_dir:
            translation = gettext.translation(
                "infinisim-launcher",
                localedir=locale_dir,
                languages=[lang],
                fallback=True
            )
        else:
            # Fallback if no locale directory found
            translation = gettext.translation(
                "infinisim-launcher",
                languages=[lang],
                fallback=True
            )
        
        translation.install()
        return lang
    except Exception as e:
        print(f"Warning: Could not load translations: {e}", file=sys.stderr)
        # Install null translation if all else fails
        gettext.install("infinisim-launcher")
        return lang

# Initialize on import
CURRENT_LANG = setup_i18n()
