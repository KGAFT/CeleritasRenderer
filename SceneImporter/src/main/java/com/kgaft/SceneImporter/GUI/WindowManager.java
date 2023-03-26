package com.kgaft.SceneImporter.GUI;

import com.formdev.flatlaf.FlatDarkLaf;
import com.kgaft.SceneImporter.GUI.SceneOutliner.SceneOutliner;

import javax.swing.*;

public class WindowManager {
    public WindowManager() throws UnsupportedLookAndFeelException {
        FlatDarkLaf.setup();
        UIManager.setLookAndFeel(new FlatDarkLaf());
        new SceneOutliner().init();
    }
}
