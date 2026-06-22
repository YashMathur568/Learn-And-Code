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
    'pbkdf2:sha256:100000:50db971fd37ea04784012d5359eee28d:c2cc45b429f785f887d6ea7e6bf5ed7bedd541e25ab02130fe6114c9b16be73b',
    1,
    1,
    NOW()
);

-- Admin profile
INSERT INTO user_profile (user_id, manager_id, department, designation)
SELECT user_id, NULL, 'Administration', 'Administrator'
FROM users WHERE username = 'admin';

-- \u2500\u2500\u2500 System config \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500
INSERT INTO system_config (config_key, config_value) VALUES
    ('llm_provider',             'gemma'),
    ('llm_api_key',              'YOUR_API_KEY_HERE'),
    ('llm_model',                'gemma-2'),
    ('gemma_llm_host',           'http://localhost:8080'),
    ('scheduler_interval_hours', '4'),
    ('max_weekly_hours',         '40');

