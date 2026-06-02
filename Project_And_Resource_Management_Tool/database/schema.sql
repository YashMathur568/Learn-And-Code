CREATE DATABASE IF NOT EXISTS prm_tool
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE prm_tool;

CREATE TABLE users (
    user_id          INT            NOT NULL AUTO_INCREMENT,
    full_name        VARCHAR(100)   NOT NULL,
    email            VARCHAR(100)   NOT NULL,
    username         VARCHAR(50)    NOT NULL,
    password_hash    VARCHAR(255)   NOT NULL,
    role             ENUM('ADMIN', 'MANAGER', 'EMPLOYEE') NOT NULL,
    is_active        TINYINT(1)     NOT NULL DEFAULT 1,
    force_pwd_change TINYINT(1)     NOT NULL DEFAULT 1,
    created_at       DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT pk_users PRIMARY KEY (user_id),
    CONSTRAINT uq_users_email    UNIQUE (email),
    CONSTRAINT uq_users_username UNIQUE (username)
);

CREATE TABLE employees (
    employee_id  INT           NOT NULL AUTO_INCREMENT,
    user_id      INT           NOT NULL,
    full_name    VARCHAR(100)  NOT NULL,
    email        VARCHAR(100)  NOT NULL,
    department   VARCHAR(100)  NOT NULL,
    designation  VARCHAR(100)  NOT NULL,
    status       ENUM('BENCH', 'ALLOCATED') NOT NULL DEFAULT 'BENCH',
    is_active    TINYINT(1)    NOT NULL DEFAULT 1,
    CONSTRAINT pk_employees        PRIMARY KEY (employee_id),
    CONSTRAINT uq_employees_user   UNIQUE (user_id),
    CONSTRAINT fk_employees_users  FOREIGN KEY (user_id) REFERENCES users (user_id)
);

CREATE TABLE employee_skills (
    skill_id     INT           NOT NULL AUTO_INCREMENT,
    employee_id  INT           NOT NULL,
    skill_name   VARCHAR(100)  NOT NULL,
    category     ENUM('Backend', 'Frontend', 'DevOps', 'QA', 'Other') NOT NULL,
    proficiency  ENUM('Beginner', 'Intermediate', 'Advanced')          NOT NULL,
    CONSTRAINT pk_employee_skills        PRIMARY KEY (skill_id),
    CONSTRAINT fk_skills_employee        FOREIGN KEY (employee_id) REFERENCES employees (employee_id)
);

CREATE TABLE projects (
    project_id   INT           NOT NULL AUTO_INCREMENT,
    name         VARCHAR(200)  NOT NULL,
    description  TEXT,
    start_date   DATE          NOT NULL,
    end_date     DATE          NOT NULL,
    status       ENUM('PLANNED', 'ACTIVE', 'ON_HOLD') NOT NULL DEFAULT 'PLANNED',
    manager_id   INT           NOT NULL,
    health       ENUM('ON_TRACK', 'ATTENTION', 'AT_RISK') NOT NULL DEFAULT 'ON_TRACK',
    CONSTRAINT pk_projects          PRIMARY KEY (project_id),
    CONSTRAINT fk_projects_manager  FOREIGN KEY (manager_id) REFERENCES employees (employee_id)
);

CREATE TABLE milestones (
    milestone_id  INT           NOT NULL AUTO_INCREMENT,
    project_id    INT           NOT NULL,
    title         VARCHAR(200)  NOT NULL,
    due_date      DATE          NOT NULL,
    status        ENUM('NOT_STARTED', 'IN_PROGRESS', 'DONE') NOT NULL DEFAULT 'NOT_STARTED',
    CONSTRAINT pk_milestones          PRIMARY KEY (milestone_id),
    CONSTRAINT fk_milestones_project  FOREIGN KEY (project_id) REFERENCES projects (project_id)
);

CREATE TABLE allocations (
    allocation_id  INT  NOT NULL AUTO_INCREMENT,
    employee_id    INT  NOT NULL,
    project_id     INT  NOT NULL,
    utilisation    INT  NOT NULL,
    from_date      DATE NOT NULL,
    to_date        DATE NOT NULL,
    is_active      TINYINT(1) NOT NULL DEFAULT 1,
    CONSTRAINT pk_allocations          PRIMARY KEY (allocation_id),
    CONSTRAINT fk_allocations_employee FOREIGN KEY (employee_id) REFERENCES employees (employee_id),
    CONSTRAINT fk_allocations_project  FOREIGN KEY (project_id)  REFERENCES projects  (project_id),
    CONSTRAINT chk_utilisation         CHECK (utilisation BETWEEN 1 AND 100)
);

CREATE TABLE timesheets (
    timesheet_id  INT       NOT NULL AUTO_INCREMENT,
    employee_id   INT       NOT NULL,
    week_start    DATE      NOT NULL,
    status        ENUM('SUBMITTED', 'MISSED') NOT NULL,
    submitted_at  DATETIME,
    CONSTRAINT pk_timesheets             PRIMARY KEY (timesheet_id),
    CONSTRAINT uq_timesheets_emp_week    UNIQUE      (employee_id, week_start),
    CONSTRAINT fk_timesheets_employee    FOREIGN KEY (employee_id) REFERENCES employees (employee_id)
);

CREATE TABLE timesheet_entries (
    entry_id      INT   NOT NULL AUTO_INCREMENT,
    timesheet_id  INT   NOT NULL,
    project_id    INT   NOT NULL,
    hours         INT   NOT NULL,
    activity_tags TEXT,
    CONSTRAINT pk_timesheet_entries         PRIMARY KEY (entry_id),
    CONSTRAINT fk_entries_timesheet         FOREIGN KEY (timesheet_id) REFERENCES timesheets (timesheet_id),
    CONSTRAINT fk_entries_project           FOREIGN KEY (project_id)   REFERENCES projects   (project_id),
    CONSTRAINT chk_entry_hours              CHECK (hours BETWEEN 1 AND 168)
);

CREATE TABLE system_config (
    config_key    VARCHAR(50) NOT NULL,
    config_value  TEXT        NOT NULL,
    CONSTRAINT pk_system_config PRIMARY KEY (config_key)
);

CREATE INDEX idx_allocations_employee_active
    ON allocations (employee_id, is_active, from_date, to_date);

CREATE INDEX idx_milestones_project_due
    ON milestones (project_id, status, due_date);

CREATE INDEX idx_entries_timesheet
    ON timesheet_entries (timesheet_id);

CREATE INDEX idx_timesheets_employee_week
    ON timesheets (employee_id, week_start);
