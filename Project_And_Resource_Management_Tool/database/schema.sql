CREATE DATABASE IF NOT EXISTS prm_tool
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE prm_tool;

-- ─── Lookup / seeded tables ───────────────────────────────────────────────────

CREATE TABLE roles (
    role_id   INT         NOT NULL AUTO_INCREMENT,
    role_name VARCHAR(50) NOT NULL,
    CONSTRAINT pk_roles      PRIMARY KEY (role_id),
    CONSTRAINT uq_roles_name UNIQUE      (role_name)
);

CREATE TABLE permissions (
    permission_id   INT          NOT NULL AUTO_INCREMENT,
    permission_name VARCHAR(100) NOT NULL,
    CONSTRAINT pk_permissions      PRIMARY KEY (permission_id),
    CONSTRAINT uq_permissions_name UNIQUE      (permission_name)
);

CREATE TABLE role_permissions (
    role_id       INT      NOT NULL,
    permission_id INT      NOT NULL,
    expires_at    DATETIME NULL,
    CONSTRAINT pk_role_permissions PRIMARY KEY (role_id, permission_id),
    CONSTRAINT fk_rp_role          FOREIGN KEY (role_id)       REFERENCES roles       (role_id),
    CONSTRAINT fk_rp_permission    FOREIGN KEY (permission_id) REFERENCES permissions (permission_id)
);

-- ─── Core user identity ────────────────────────────────────────────────────────

CREATE TABLE users (
    user_id             INT          NOT NULL AUTO_INCREMENT,
    full_name           VARCHAR(100) NOT NULL,
    email               VARCHAR(100) NOT NULL,
    username            VARCHAR(50)  NOT NULL,
    password_hash       VARCHAR(255) NOT NULL,
    role_id             INT          NOT NULL,
    is_active           TINYINT(1)   NOT NULL DEFAULT 1,
    is_frozen           TINYINT(1)   NOT NULL DEFAULT 0,
    password_expires_at DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    created_at          DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT pk_users          PRIMARY KEY (user_id),
    CONSTRAINT uq_users_email    UNIQUE      (email),
    CONSTRAINT uq_users_username UNIQUE      (username),
    CONSTRAINT fk_users_role     FOREIGN KEY (role_id) REFERENCES roles (role_id)
);

-- \u2500\u2500\u2500 User profile (all roles) \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500

CREATE TABLE user_profile (
    user_id     INT          NOT NULL,
    manager_id  INT          NULL,
    department  VARCHAR(100) NOT NULL,
    designation VARCHAR(100) NOT NULL,
    CONSTRAINT pk_user_profile       PRIMARY KEY (user_id),
    CONSTRAINT fk_uprof_user          FOREIGN KEY (user_id)    REFERENCES users (user_id),
    CONSTRAINT fk_uprof_manager       FOREIGN KEY (manager_id) REFERENCES users (user_id)
);

CREATE TABLE resource_status (
    user_id  INT                        NOT NULL,
    status   ENUM('BENCH', 'ALLOCATED') NOT NULL DEFAULT 'BENCH',
    CONSTRAINT pk_resource_status PRIMARY KEY (user_id),
    CONSTRAINT fk_rstatus_user    FOREIGN KEY (user_id) REFERENCES users (user_id)
);

-- \u2500\u2500\u2500 Skills \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500

CREATE TABLE user_skills (
    skill_id     INT          NOT NULL AUTO_INCREMENT,
    user_id      INT          NOT NULL,
    skill_name   VARCHAR(100) NOT NULL,
    category     ENUM('Backend', 'Frontend', 'DevOps', 'QA', 'Other') NOT NULL,
    proficiency  ENUM('Beginner', 'Intermediate', 'Advanced')          NOT NULL,
    CONSTRAINT pk_user_skills PRIMARY KEY (skill_id),
    CONSTRAINT fk_skills_user FOREIGN KEY (user_id) REFERENCES users (user_id)
);

-- \u2500\u2500\u2500 Projects + milestones \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500

CREATE TABLE projects (
    project_id   INT          NOT NULL AUTO_INCREMENT,
    name         VARCHAR(200) NOT NULL,
    description  TEXT,
    start_date   DATE         NOT NULL,
    end_date     DATE         NOT NULL,
    status                ENUM('PLANNED', 'ACTIVE', 'ON_HOLD')      NOT NULL DEFAULT 'PLANNED',
    manager_id            INT          NOT NULL,
    health                ENUM('ON_TRACK', 'ATTENTION', 'AT_RISK')  NOT NULL DEFAULT 'ON_TRACK',
    total_story_points    INT          NOT NULL DEFAULT 0,
    completed_story_points INT         NOT NULL DEFAULT 0,
    CONSTRAINT pk_projects         PRIMARY KEY (project_id),
    CONSTRAINT fk_projects_manager FOREIGN KEY (manager_id) REFERENCES users (user_id)
);

CREATE TABLE milestones (
    milestone_id  INT          NOT NULL AUTO_INCREMENT,
    project_id    INT          NOT NULL,
    title         VARCHAR(200) NOT NULL,
    due_date      DATE         NOT NULL,
    status        ENUM('NOT_STARTED', 'IN_PROGRESS', 'DONE') NOT NULL DEFAULT 'NOT_STARTED',
    CONSTRAINT pk_milestones         PRIMARY KEY (milestone_id),
    CONSTRAINT fk_milestones_project FOREIGN KEY (project_id) REFERENCES projects (project_id)
);

-- \u2500\u2500\u2500 Allocations \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500

CREATE TABLE allocations (
    allocation_id  INT  NOT NULL AUTO_INCREMENT,
    user_id        INT  NOT NULL,
    project_id     INT  NOT NULL,
    utilisation    INT  NOT NULL,
    from_date      DATE NOT NULL,
    `to_date`      DATE NOT NULL,
    is_active      TINYINT(1) NOT NULL DEFAULT 1,
    CONSTRAINT pk_allocations          PRIMARY KEY (allocation_id),
    CONSTRAINT fk_allocations_user     FOREIGN KEY (user_id)    REFERENCES users    (user_id),
    CONSTRAINT fk_allocations_project  FOREIGN KEY (project_id) REFERENCES projects (project_id),
    CONSTRAINT chk_utilisation         CHECK (utilisation BETWEEN 1 AND 100)
);

-- \u2500\u2500\u2500 Timesheets \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500

CREATE TABLE timesheets (
    timesheet_id  INT      NOT NULL AUTO_INCREMENT,
    user_id       INT      NOT NULL,
    week_start    DATE     NOT NULL,
    status        ENUM('SUBMITTED', 'MISSED') NOT NULL,
    submitted_at  DATETIME,
    CONSTRAINT pk_timesheets         PRIMARY KEY (timesheet_id),
    CONSTRAINT uq_timesheets_user_wk UNIQUE      (user_id, week_start),
    CONSTRAINT fk_timesheets_user    FOREIGN KEY (user_id) REFERENCES users (user_id)
);

CREATE TABLE timesheet_entries (
    entry_id      INT  NOT NULL AUTO_INCREMENT,
    timesheet_id  INT  NOT NULL,
    project_id    INT  NOT NULL,
    hours         INT  NOT NULL,
    activity_tags TEXT,
    CONSTRAINT pk_timesheet_entries PRIMARY KEY (entry_id),
    CONSTRAINT fk_entries_timesheet FOREIGN KEY (timesheet_id) REFERENCES timesheets (timesheet_id),
    CONSTRAINT fk_entries_project   FOREIGN KEY (project_id)   REFERENCES projects   (project_id),
    CONSTRAINT chk_entry_hours      CHECK (hours BETWEEN 1 AND 168)
);

CREATE TABLE timesheet_reminder_log (
    user_id           INT         NOT NULL,
    week_start        DATE        NOT NULL,
    reminder1_sent_at DATETIME    DEFAULT NULL,
    reminder2_sent_at DATETIME    DEFAULT NULL,
    frozen_at         DATETIME    DEFAULT NULL,
    CONSTRAINT pk_timesheet_reminder_log PRIMARY KEY (user_id, week_start),
    CONSTRAINT fk_reminder_log_user      FOREIGN KEY (user_id) REFERENCES users (user_id)
);

CREATE TABLE system_config (
    config_key   VARCHAR(50) NOT NULL,
    config_value TEXT        NOT NULL,
    CONSTRAINT pk_system_config PRIMARY KEY (config_key)
);

-- \u2500\u2500\u2500 Indexes \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500

CREATE INDEX idx_allocations_user_active
    ON allocations (user_id, is_active, from_date, `to_date`);

CREATE INDEX idx_milestones_project_due
    ON milestones (project_id, status, due_date);

CREATE INDEX idx_entries_timesheet
    ON timesheet_entries (timesheet_id);

CREATE INDEX idx_timesheets_user_week
    ON timesheets (user_id, week_start);

CREATE INDEX idx_user_profile_manager
    ON user_profile (manager_id);

CREATE INDEX idx_role_permissions_role
    ON role_permissions (role_id, expires_at);

-- \u2500\u2500\u2500 Triggers \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500

DELIMITER ;;

CREATE TRIGGER trg_entry_must_be_allocated
BEFORE INSERT ON timesheet_entries
FOR EACH ROW
BEGIN
    DECLARE v_user_id INT;
    DECLARE v_count   INT;

    SELECT user_id INTO v_user_id
    FROM timesheets
    WHERE timesheet_id = NEW.timesheet_id;

    SELECT COUNT(*) INTO v_count
    FROM allocations a
    JOIN timesheets  t ON t.timesheet_id = NEW.timesheet_id
    WHERE a.user_id    = v_user_id
      AND a.project_id = NEW.project_id
      AND a.from_date <= t.week_start
      AND a.to_date   >= t.week_start;

    IF v_count = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Cannot log hours: no allocation for this project during this week';
    END IF;
END;;

DELIMITER ;
