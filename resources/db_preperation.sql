PRAGMA foreign_keys = true;

-- manager.db
CREATE TABLE projects (
    id integer PRIMARY KEY AUTOINCREMENT,
    file_name text NOT NULL
);

CREATE TABLE templates (
    id integer PRIMARY KEY AUTOINCREMENT,
    proj_id integer REFERENCES projects(id) on DELETE SET NULL,
    file_name text NOT NULL
    UNIQUE(proj_id, file_name)
);

-- INSERT INTO projects (file_name) VALUES ('sntrpg_skills.db');
