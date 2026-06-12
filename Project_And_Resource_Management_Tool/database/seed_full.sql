USE prm_tool;

-- ═══════════════════════════════════════════════════════════════════════
-- FULL TEST SEED  —  password for ALL users: Test@1234
-- password_expires_at = NOW() forces change on first login
-- ═══════════════════════════════════════════════════════════════════════

SET @PH = 'pbkdf2:sha256:100000:aabbccddeeff00112233445566778899:2b4c0f22d979fabb8e172fb55d03f9fad7e20d4714a4d4f6ab58f1d35619e33a';

-- ── 0. Update every existing user's password to Test@1234 ───────────────
UPDATE users SET password_hash = @PH, password_expires_at = NOW();

-- ── 1. New admins (IDs 13-14) ────────────────────────────────────────────
INSERT INTO users (user_id, full_name, email, username, password_hash, role_id, is_active, password_expires_at) VALUES
  (13, 'Prateek Singh',  'prateek.singh@prm.local',  'prateek568', @PH, 1, 1, NOW()),
  (14, 'Meera Nair',     'meera.nair@prm.local',     'meera568',   @PH, 1, 1, NOW());

-- ── 2. New managers (IDs 15-17) ──────────────────────────────────────────
INSERT INTO users (user_id, full_name, email, username, password_hash, role_id, is_active, password_expires_at) VALUES
  (15, 'Sanjay Khanna',  'sanjay.khanna@prm.local',  'sanjay568',  @PH, 2, 1, NOW()),
  (16, 'Pooja Mehta',    'pooja.mehta@prm.local',    'pooja568',   @PH, 2, 1, NOW()),
  (17, 'Rahul Verma',    'rahul.verma@prm.local',    'rahul568',   @PH, 2, 1, NOW());

INSERT INTO resource_profile (user_id, manager_id, department, designation) VALUES
  (15, NULL, 'Delivery', 'Senior Delivery Manager'),
  (16, NULL, 'Delivery', 'Project Manager'),
  (17, NULL, 'Engineering', 'Engineering Manager');

-- ── 3. New resources (IDs 18-32) ─────────────────────────────────────────
INSERT INTO users (user_id, full_name, email, username, password_hash, role_id, is_active, password_expires_at) VALUES
  (18, 'Vikram Rao',      'vikram.rao@prm.local',      'vikram568',   @PH, 3, 1, NOW()),
  (19, 'Sonia Gupta',     'sonia.gupta@prm.local',     'sonia568',    @PH, 3, 1, NOW()),
  (20, 'Rohit Jain',      'rohit.jain@prm.local',      'rohit568',    @PH, 3, 1, NOW()),
  (21, 'Pallavi Desai',   'pallavi.desai@prm.local',   'pallavi568',  @PH, 3, 1, NOW()),
  (22, 'Manish Kumar',    'manish.kumar@prm.local',    'manish568',   @PH, 3, 1, NOW()),
  (23, 'Sneha Reddy',     'sneha.reddy@prm.local',     'sneha568',    @PH, 3, 1, NOW()),
  (24, 'Aakash Nair',     'aakash.nair@prm.local',     'aakash568',   @PH, 3, 1, NOW()),
  (25, 'Divya Iyer',      'divya.iyer@prm.local',      'divya568',    @PH, 3, 1, NOW()),
  (26, 'Suresh Pillai',   'suresh.pillai@prm.local',   'suresh568',   @PH, 3, 1, NOW()),
  (27, 'Kavita Sharma',   'kavita.sharma@prm.local',   'kavita568',   @PH, 3, 1, NOW()),
  (28, 'Nitin Bajaj',     'nitin.bajaj@prm.local',     'nitin568',    @PH, 3, 1, NOW()),
  (29, 'Swati Agarwal',   'swati.agarwal@prm.local',   'swati568',    @PH, 3, 1, NOW()),
  (30, 'Harish Menon',    'harish.menon@prm.local',    'harish568',   @PH, 3, 1, NOW()),
  (31, 'Ayesha Khan',     'ayesha.khan@prm.local',     'ayesha568',   @PH, 3, 1, NOW()),
  (32, 'Rajan Tiwari',    'rajan.tiwari@prm.local',    'rajan568',    @PH, 3, 1, NOW());

-- resource_profile  (manager_id → which manager owns this resource)
INSERT INTO resource_profile (user_id, manager_id, department, designation) VALUES
  -- Sanjay's team (15)
  (18, 15, 'Engineering',             'Senior Backend Developer'),
  (19, 15, 'Engineering',             'Frontend Developer'),
  (20, 15, 'Engineering',             'Backend Developer'),
  (26, 15, 'Engineering',             'Lead Backend Developer'),
  (28, 15, 'DevOps / Infrastructure', 'DevOps Engineer'),
  -- Pooja's team (16)
  (21, 16, 'DevOps / Infrastructure', 'Cloud Engineer'),
  (23, 16, 'Engineering',             'Full-Stack Developer'),
  (25, 16, 'Data Engineering',        'ML Engineer'),
  (29, 16, 'Engineering',             'Frontend Developer'),
  (30, 16, 'Engineering',             'Full-Stack Developer'),
  -- Rahul's team (17)
  (22, 17, 'Engineering',             'Backend Developer'),
  (24, 17, 'Mobile',                  'Mobile Developer'),
  (27, 17, 'Quality Assurance',       'Senior QA Engineer'),
  (31, 17, 'Data Engineering',        'Data Analyst'),
  (32, 17, 'Mobile',                  'iOS Developer');

-- resource_status
INSERT INTO resource_status (user_id, status) VALUES
  (18, 'ALLOCATED'),
  (19, 'ALLOCATED'),
  (20, 'ALLOCATED'),
  (21, 'ALLOCATED'),
  (22, 'ALLOCATED'),
  (23, 'ALLOCATED'),
  (24, 'BENCH'),
  (25, 'ALLOCATED'),
  (26, 'ALLOCATED'),
  (27, 'ALLOCATED'),
  (28, 'ALLOCATED'),
  (29, 'BENCH'),
  (30, 'ALLOCATED'),
  (31, 'BENCH'),
  (32, 'BENCH');

-- ── 4. Skills ─────────────────────────────────────────────────────────────
INSERT INTO user_skills (user_id, skill_name, category, proficiency) VALUES
  -- Vikram Rao (18) — Java backend
  (18, 'Java',           'Backend',  'Advanced'),
  (18, 'Spring Boot',    'Backend',  'Advanced'),
  (18, 'Hibernate',      'Backend',  'Intermediate'),
  (18, 'SQL',            'Backend',  'Intermediate'),
  -- Sonia Gupta (19) — Frontend
  (19, 'React',          'Frontend', 'Advanced'),
  (19, 'Vue.js',         'Frontend', 'Intermediate'),
  (19, 'TypeScript',     'Frontend', 'Advanced'),
  (19, 'Angular',        'Frontend', 'Beginner'),
  -- Rohit Jain (20) — Python backend
  (20, 'Python',         'Backend',  'Advanced'),
  (20, 'FastAPI',        'Backend',  'Intermediate'),
  (20, 'Django',         'Backend',  'Intermediate'),
  (20, 'PostgreSQL',     'Backend',  'Intermediate'),
  -- Pallavi Desai (21) — Cloud
  (21, 'AWS',            'DevOps',   'Advanced'),
  (21, 'Azure',          'DevOps',   'Intermediate'),
  (21, 'Terraform',      'DevOps',   'Intermediate'),
  (21, 'Docker',         'DevOps',   'Advanced'),
  -- Manish Kumar (22) — Node/GraphQL
  (22, 'Node.js',        'Backend',  'Advanced'),
  (22, 'GraphQL',        'Backend',  'Intermediate'),
  (22, 'MongoDB',        'Backend',  'Advanced'),
  (22, 'Redis',          'Backend',  'Beginner'),
  -- Sneha Reddy (23) — .NET
  (23, 'C#',             'Backend',  'Advanced'),
  (23, '.NET',           'Backend',  'Advanced'),
  (23, 'SQL Server',     'Backend',  'Intermediate'),
  (23, 'Azure',          'DevOps',   'Beginner'),
  -- Aakash Nair (24) — Mobile
  (24, 'React Native',   'Frontend', 'Advanced'),
  (24, 'Flutter',        'Frontend', 'Intermediate'),
  (24, 'JavaScript',     'Frontend', 'Advanced'),
  -- Divya Iyer (25) — ML/AI
  (25, 'Python',         'Backend',  'Advanced'),
  (25, 'PyTorch',        'Other',    'Advanced'),
  (25, 'TensorFlow',     'Other',    'Intermediate'),
  (25, 'Machine Learning', 'Other',  'Advanced'),
  -- Suresh Pillai (26) — Java/Kafka
  (26, 'Java',           'Backend',  'Advanced'),
  (26, 'Kafka',          'Backend',  'Advanced'),
  (26, 'Microservices',  'Backend',  'Advanced'),
  (26, 'Spring Boot',    'Backend',  'Advanced'),
  -- Kavita Sharma (27) — QA
  (27, 'Selenium',       'QA',       'Advanced'),
  (27, 'TestNG',         'QA',       'Advanced'),
  (27, 'JUnit',          'QA',       'Intermediate'),
  (27, 'Postman',        'QA',       'Intermediate'),
  -- Nitin Bajaj (28) — DevOps
  (28, 'Kubernetes',     'DevOps',   'Advanced'),
  (28, 'Helm',           'DevOps',   'Advanced'),
  (28, 'Terraform',      'DevOps',   'Intermediate'),
  (28, 'Jenkins',        'DevOps',   'Advanced'),
  -- Swati Agarwal (29) — Frontend
  (29, 'React',          'Frontend', 'Advanced'),
  (29, 'Redux',          'Frontend', 'Intermediate'),
  (29, 'TypeScript',     'Frontend', 'Advanced'),
  (29, 'CSS',            'Frontend', 'Advanced'),
  -- Harish Menon (30) — Full Stack
  (30, 'Angular',        'Frontend', 'Advanced'),
  (30, 'Java',           'Backend',  'Intermediate'),
  (30, 'Spring Boot',    'Backend',  'Beginner'),
  (30, 'SQL',            'Backend',  'Intermediate'),
  -- Ayesha Khan (31) — Data
  (31, 'Python',         'Backend',  'Intermediate'),
  (31, 'SQL',            'Backend',  'Advanced'),
  (31, 'Pandas',         'Other',    'Intermediate'),
  (31, 'Power BI',       'Other',    'Beginner'),
  -- Rajan Tiwari (32) — iOS
  (32, 'Swift',          'Other',    'Advanced'),
  (32, 'Objective-C',    'Other',    'Intermediate'),
  (32, 'iOS',            'Other',    'Advanced'),
  (32, 'Xcode',          'Other',    'Intermediate');

-- ── 5. Projects ───────────────────────────────────────────────────────────
INSERT INTO projects (project_id, name, description, start_date, end_date, status, manager_id, health, total_story_points, completed_story_points) VALUES
  (4, 'FinEdge Banking App',      'Core banking platform modernisation with microservices',              '2026-01-01', '2026-12-31', 'ACTIVE',   15, 'ATTENTION', 120, 68),
  (5, 'HealthBridge Platform',    'Integrated health records and patient management system',              '2026-03-01', '2026-11-30', 'ACTIVE',   16, 'ON_TRACK',   80, 32),
  (6, 'EduLearn LMS',             'Learning Management System for enterprise training',                   '2026-02-01', '2026-09-30', 'ON_HOLD',  17, 'AT_RISK',    60, 14),
  (7, 'RetailVision Analytics',   'Real-time retail analytics with ML-powered dashboards',               '2026-04-01', '2027-03-31', 'ACTIVE',   15, 'ON_TRACK',  100, 42),
  (8, 'CloudMigrate Pro',         'Enterprise cloud migration from on-prem to multi-cloud',              '2026-08-01', '2027-06-30', 'PLANNED',   6, 'ON_TRACK',    0,  0);

-- ── 6. Milestones ─────────────────────────────────────────────────────────
INSERT INTO milestones (project_id, title, due_date, status) VALUES
  -- Tera2 (1)
  (1, 'Requirements Finalised',     '2026-02-28', 'DONE'),
  (1, 'Backend Development Phase 1','2026-06-30', 'IN_PROGRESS'),
  (1, 'Integration & UAT',          '2026-09-30', 'NOT_STARTED'),
  -- Alpha Project (2)
  (2, 'Architecture Design',        '2026-03-15', 'DONE'),
  (2, 'MVP Delivery',               '2026-07-31', 'IN_PROGRESS'),
  -- TechNova (3)
  (3, 'Design System Complete',     '2026-05-15', 'DONE'),
  (3, 'Backend APIs v1',            '2026-07-31', 'IN_PROGRESS'),
  (3, 'Frontend Integration',       '2026-09-30', 'NOT_STARTED'),
  (3, 'Performance Testing',        '2026-11-30', 'NOT_STARTED'),
  -- FinEdge (4)
  (4, 'Discovery & Scoping',        '2026-02-15', 'DONE'),
  (4, 'Core Banking Module',        '2026-07-15', 'IN_PROGRESS'),
  (4, 'Compliance & Security Audit','2026-10-31', 'NOT_STARTED'),
  (4, 'Go-Live',                    '2026-12-15', 'NOT_STARTED'),
  -- HealthBridge (5)
  (5, 'UX Research & Wireframes',   '2026-04-30', 'DONE'),
  (5, 'Patient Module',             '2026-08-31', 'IN_PROGRESS'),
  (5, 'Billing Integration',        '2026-10-31', 'NOT_STARTED'),
  -- EduLearn (6)
  (6, 'Requirement Gathering',      '2026-03-31', 'DONE'),
  (6, 'Course Engine Build',        '2026-06-30', 'IN_PROGRESS'),
  (6, 'User Acceptance Testing',    '2026-09-15', 'NOT_STARTED'),
  -- RetailVision (7)
  (7, 'Data Pipeline Setup',        '2026-06-30', 'IN_PROGRESS'),
  (7, 'Analytics Dashboard v1',     '2026-09-30', 'NOT_STARTED'),
  (7, 'ML Model Integration',       '2026-12-31', 'NOT_STARTED');

-- ── 7. Allocations ────────────────────────────────────────────────────────
-- (Existing: Ananya-10 → P3, Neha-12 → P3 already exist)
INSERT INTO allocations (user_id, project_id, utilisation, from_date, `to_date`, is_active) VALUES
  -- TechNova (3) — manager Ravi (6)
  (19, 3,  80, '2026-05-01', '2026-10-31', 1),
  (27, 3,  80, '2026-05-01', '2026-10-31', 1),
  -- FinEdge (4) — manager Sanjay (15)
  (18, 4, 100, '2026-01-15', '2026-12-31', 1),
  (20, 4, 100, '2026-02-01', '2026-12-31', 1),
  (26, 4, 100, '2026-01-15', '2026-12-31', 1),
  (28, 4, 100, '2026-04-01', '2026-12-31', 1),
  -- HealthBridge (5) — manager Pooja (16)
  (21, 5, 100, '2026-03-01', '2026-11-30', 1),
  (23, 5, 100, '2026-03-01', '2026-11-30', 1),
  (30, 5, 100, '2026-03-01', '2026-11-30', 1),
  -- RetailVision (7) — manager Sanjay (15)
  (22, 7,  80, '2026-04-01', '2027-03-31', 1),
  (25, 7,  80, '2026-04-01', '2027-03-31', 1);

-- ── 8. Timesheets  (4 recent weeks per allocated employee) ────────────────
-- Weeks: 2026-05-18, 2026-05-25, 2026-06-01, 2026-06-08
-- Employees with earlier start (since Jan-Apr): Vikram(18), Rohit(20), Suresh(26),
--   Manish(22), Divya(25), Pallavi(21), Sneha(23), Harish(30), Nitin(28)
-- Employees from May: Sonia(19), Kavita(27)
-- Employees from June: Ananya(10), Neha(12)

-- Vikram (18) → project 4
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (18, '2026-04-27', 'MISSED',    NULL),
  (18, '2026-05-18', 'SUBMITTED', '2026-05-23 10:00:00'),
  (18, '2026-05-25', 'SUBMITTED', '2026-05-30 09:30:00'),
  (18, '2026-06-01', 'SUBMITTED', '2026-06-05 11:00:00'),
  (18, '2026-06-08', 'SUBMITTED', '2026-06-12 08:45:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 4, 38, 'Backend Development, API Design'
FROM timesheets t WHERE t.user_id=18 AND t.status='SUBMITTED';

-- Rohit (20) → project 4
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (20, '2026-05-18', 'SUBMITTED', '2026-05-22 09:00:00'),
  (20, '2026-05-25', 'SUBMITTED', '2026-05-29 10:15:00'),
  (20, '2026-06-01', 'SUBMITTED', '2026-06-04 14:00:00'),
  (20, '2026-06-08', 'SUBMITTED', '2026-06-11 17:30:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 4, 40, 'API Development, Code Review'
FROM timesheets t WHERE t.user_id=20 AND t.status='SUBMITTED';

-- Suresh (26) → project 4
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (26, '2026-05-18', 'SUBMITTED', '2026-05-23 11:00:00'),
  (26, '2026-05-25', 'SUBMITTED', '2026-05-30 11:00:00'),
  (26, '2026-06-01', 'MISSED',    NULL),
  (26, '2026-06-08', 'SUBMITTED', '2026-06-12 09:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 4, 40, 'Kafka Integration, Microservices'
FROM timesheets t WHERE t.user_id=26 AND t.status='SUBMITTED';

-- Nitin (28) → project 4
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (28, '2026-05-18', 'SUBMITTED', '2026-05-22 16:00:00'),
  (28, '2026-05-25', 'SUBMITTED', '2026-05-29 16:00:00'),
  (28, '2026-06-01', 'SUBMITTED', '2026-06-04 10:00:00'),
  (28, '2026-06-08', 'SUBMITTED', '2026-06-12 10:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 4, 35, 'CI/CD Pipeline, Kubernetes Config'
FROM timesheets t WHERE t.user_id=28 AND t.status='SUBMITTED';

-- Pallavi (21) → project 5
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (21, '2026-05-18', 'SUBMITTED', '2026-05-22 12:00:00'),
  (21, '2026-05-25', 'SUBMITTED', '2026-05-29 12:00:00'),
  (21, '2026-06-01', 'SUBMITTED', '2026-06-05 09:00:00'),
  (21, '2026-06-08', 'SUBMITTED', '2026-06-11 16:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 5, 38, 'Cloud Infrastructure, Terraform Scripts'
FROM timesheets t WHERE t.user_id=21 AND t.status='SUBMITTED';

-- Sneha (23) → project 5
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (23, '2026-04-27', 'MISSED',    NULL),
  (23, '2026-05-18', 'SUBMITTED', '2026-05-23 14:00:00'),
  (23, '2026-05-25', 'SUBMITTED', '2026-05-30 14:00:00'),
  (23, '2026-06-01', 'SUBMITTED', '2026-06-06 09:30:00'),
  (23, '2026-06-08', 'SUBMITTED', '2026-06-12 12:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 5, 40, '.NET API Development, Bug Fixes'
FROM timesheets t WHERE t.user_id=23 AND t.status='SUBMITTED';

-- Harish (30) → project 5
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (30, '2026-05-18', 'SUBMITTED', '2026-05-22 10:00:00'),
  (30, '2026-05-25', 'SUBMITTED', '2026-05-29 10:00:00'),
  (30, '2026-06-01', 'SUBMITTED', '2026-06-04 12:00:00'),
  (30, '2026-06-08', 'SUBMITTED', '2026-06-12 15:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 5, 36, 'Angular Components, Integration Testing'
FROM timesheets t WHERE t.user_id=30 AND t.status='SUBMITTED';

-- Manish (22) → project 7
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (22, '2026-05-18', 'SUBMITTED', '2026-05-21 17:00:00'),
  (22, '2026-05-25', 'SUBMITTED', '2026-05-28 17:00:00'),
  (22, '2026-06-01', 'SUBMITTED', '2026-06-04 16:00:00'),
  (22, '2026-06-08', 'SUBMITTED', '2026-06-11 18:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 7, 32, 'GraphQL Schema, Data Aggregation'
FROM timesheets t WHERE t.user_id=22 AND t.status='SUBMITTED';

-- Divya (25) → project 7
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (25, '2026-05-18', 'SUBMITTED', '2026-05-22 15:00:00'),
  (25, '2026-05-25', 'SUBMITTED', '2026-05-29 15:00:00'),
  (25, '2026-06-01', 'MISSED',    NULL),
  (25, '2026-06-08', 'SUBMITTED', '2026-06-12 14:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 7, 36, 'ML Model Training, Feature Engineering'
FROM timesheets t WHERE t.user_id=25 AND t.status='SUBMITTED';

-- Sonia (19) → project 3  (allocated from May 1, skip May 18 to test gap)
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (19, '2026-05-25', 'SUBMITTED', '2026-05-30 10:00:00'),
  (19, '2026-06-01', 'SUBMITTED', '2026-06-06 10:00:00'),
  (19, '2026-06-08', 'SUBMITTED', '2026-06-12 11:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 3, 40, 'React Components, UI Integration'
FROM timesheets t WHERE t.user_id=19 AND t.status='SUBMITTED';

-- Kavita (27) → project 3
INSERT INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (27, '2026-05-25', 'SUBMITTED', '2026-05-30 16:00:00'),
  (27, '2026-06-01', 'SUBMITTED', '2026-06-05 16:00:00'),
  (27, '2026-06-08', 'SUBMITTED', '2026-06-12 16:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 3, 38, 'Test Automation, Regression Testing'
FROM timesheets t WHERE t.user_id=27 AND t.status='SUBMITTED';

-- Ananya (10) → project 3 (already has some; add June 8 if missing)
INSERT IGNORE INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (10, '2026-06-08', 'SUBMITTED', '2026-06-12 09:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 3, 35, 'Docker Deployment, K8s Config'
FROM timesheets t WHERE t.user_id=10 AND t.week_start='2026-06-08';

-- Neha (12) → project 3
INSERT IGNORE INTO timesheets (user_id, week_start, status, submitted_at) VALUES
  (12, '2026-06-08', 'SUBMITTED', '2026-06-12 10:00:00');

INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags)
SELECT t.timesheet_id, 3, 32, 'Data Pipeline, Model Evaluation'
FROM timesheets t WHERE t.user_id=12 AND t.week_start='2026-06-08';
