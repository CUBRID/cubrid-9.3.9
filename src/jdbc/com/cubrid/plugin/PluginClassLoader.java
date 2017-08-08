package com.cubrid.plugin;

import java.io.File;
import java.io.FileFilter;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.HashMap;

public class PluginClassLoader extends URLClassLoader {
	private static PluginClassLoader instance = null;

	private HashMap<String, Long> files = new HashMap<String, Long>();

	private File root;

	private PluginClassLoader() {
		super(new URL[0]);
		init();
	}

	private void init() {
		root = new File(PluginServer.getPluginPath());
		File[] jars = root.listFiles(new FileFilter() {
			public boolean accept(File f) {
				if (f.getName().lastIndexOf(".jar") > 0) {
					return true;
				}
				return false;
			}
		});

		for (int i = 0; i < jars.length; i++) {
			files.put(jars[i].getName(), new Long(jars[i].lastModified()));
		}

		File[] classes = root.listFiles(new FileFilter() {
			public boolean accept(File f) {
				if (f.getName().lastIndexOf(".class") > 0) {
					return true;
				}
				return false;
			}
		});

		for (int i = 0; i < classes.length; i++) {
			files.put(classes[i].getName(), new Long(classes[i].lastModified()));
		}

		try {
			addURL(root.toURI().toURL());
			for (int i = 0; i < jars.length; i++) {
				addURL(jars[i].toURI().toURL());
			}
		} catch (MalformedURLException e) {
			PluginServer.log(e);
		}
	}

	public Class<?> loadClass(String name) throws ClassNotFoundException {
		try {
			if (!modified()) {
				return super.loadClass(name);
			}
		} catch (MalformedURLException e) {
			PluginServer.log(e);
		}

		instance = new PluginClassLoader();
		return instance.loadClass(name);
	}

	private boolean modified() throws MalformedURLException {
		File[] files = root.listFiles(new FileFilter() {
			public boolean accept(File f) {
				if (f.getName().lastIndexOf(".jar") > 0) {
					return true;
				}
				if (f.getName().lastIndexOf(".class") > 0) {
					return true;
				}
				return false;
			}
		});

		if (this.files.size() != files.length) {
			return true;
		}

		for (int i = 0; i < files.length; i++) {
			if (!this.files.containsKey(files[i].getName())) {
				return true;
			}

			long l = ((Long) this.files.get(files[i].getName())).longValue();
			if (files[i].lastModified() != l) {
				return true;
			}
		}

		return false;
	}

	public static PluginClassLoader getInstance() {
		if (instance == null) {
			instance = new PluginClassLoader();
		}

		return instance;
	}
}
