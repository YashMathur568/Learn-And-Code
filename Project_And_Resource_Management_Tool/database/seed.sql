USE prm_tool;

INSERT INTO users (full_name, email, username, password_hash, role, is_active, force_pwd_change)
VALUES (
    'System Admin',
    'admin@prm.local',
    'admin',
    'pbkdf2:sha256:100000:1a2b3c4d5e6f708192a3b4c5d6e7f809:cf348f899d6fb6d3a5f4b32e0df23e5e7e610de081695359f15f09c523de3e60',
    'ADMIN',
    1,
    1
);

INSERT INTO system_config (config_key, config_value) VALUES
    ('llm_provider',               'gemini'),
    ('scheduler_interval_hours',   '4'),
    ('max_weekly_hours',           '40');
