USE prm_tool;

INSERT INTO users (full_name, email, username, password_hash, role, is_active, force_pwd_change)
VALUES (
    'System Admin',
    'admin@prm.local',
    'admin',
    '$2b$12$0n.fAWOlDQg1pbdPTHIQXOPVoUw6Dt5NFPdMGa3uboTEBwD3.c2N6',
    'ADMIN',
    1,
    1
);

INSERT INTO system_config (config_key, config_value) VALUES
    ('llm_provider',               'gemini'),
    ('scheduler_interval_hours',   '4'),
    ('max_weekly_hours',           '40');
