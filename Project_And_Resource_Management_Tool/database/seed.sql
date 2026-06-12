USE prm_tool;

-- \u2500\u2500\u2500 Roles \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500
INSERT INTO roles (role_name) VALUES ('ADMIN'), ('MANAGER'), ('RESOURCE');

-- \u2500\u2500\u2500 Permissions \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500
INSERT INTO permissions (permission_name) VALUES
    ('CHANGE_PASSWORD'),
    ('MANAGE_USERS'),
    ('MANAGE_PROFILES'),
    ('MANAGE_PROJECTS'),
    ('MANAGE_SKILLS'),
    ('ALLOCATE_RESOURCES'),
    ('SUBMIT_TIMESHEET'),
    ('VIEW_TEAM_TIMESHEETS'),
    ('MANAGE_CONFIG'),
    ('USE_AI_FEATURES');

-- \u2500\u2500\u2500 Default role \u2192 permission grants \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500

-- ADMIN: manage everything except AI and resource-only actions
INSERT INTO role_permissions (role_id, permission_id, expires_at)
SELECT 1, permission_id, NULL FROM permissions
WHERE permission_name IN (
    'CHANGE_PASSWORD', 'MANAGE_USERS', 'MANAGE_PROFILES',
    'MANAGE_PROJECTS', 'MANAGE_SKILLS', 'VIEW_TEAM_TIMESHEETS', 'MANAGE_CONFIG'
);

-- MANAGER: project + allocation + timesheet management + AI
INSERT INTO role_permissions (role_id, permission_id, expires_at)
SELECT 2, permission_id, NULL FROM permissions
WHERE permission_name IN (
    'CHANGE_PASSWORD', 'MANAGE_PROJECTS', 'ALLOCATE_RESOURCES',
    'SUBMIT_TIMESHEET', 'VIEW_TEAM_TIMESHEETS', 'USE_AI_FEATURES'
);

-- RESOURCE: submit timesheets only (CHANGE_PASSWORD NOT granted by default)
INSERT INTO role_permissions (role_id, permission_id, expires_at)
SELECT 3, permission_id, NULL FROM permissions
WHERE permission_name IN ('SUBMIT_TIMESHEET');

-- \u2500\u2500\u2500 Admin user \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500
-- password_expires_at = NOW() forces password change on first login
-- password = Admin@1234 (pbkdf2:sha256 hash)
INSERT INTO users (full_name, email, username, password_hash, role_id, is_active, password_expires_at)
VALUES (
    'System Admin',
    'admin@prm.local',
    'admin',
    'pbkdf2:sha256:100000:1a2b3c4d5e6f708192a3b4c5d6e7f809:cf348f899d6fb6d3a5f4b32e0df23e5e7e610de081695359f15f09c523de3e60',
    1,
    1,
    NOW()
);

-- \u2500\u2500\u2500 System config \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500
INSERT INTO system_config (config_key, config_value) VALUES
    ('llm_provider',             'gemini'),
    ('scheduler_interval_hours', '4'),
    ('max_weekly_hours',         '40');

