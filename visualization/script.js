// Инициализация Gantt
gantt.init("gantt_container");

// Ваши данные
const scheduleData = [
    {
        "end": "17.08.25_12:00:00",
        "executor": 1,
        "operation": 101,
        "start": "17.08.25_10:00:00"
    },
    {
        "end": "17.08.25_15:00:00",
        "executor": 1,
        "operation": 102,
        "start": "17.08.25_13:00:00"
    },
    {
        "end": "17.08.25_11:30:00",
        "executor": 2,
        "operation": 201,
        "start": "17.08.25_09:00:00"
    }
];

// Преобразование данных
function prepareData(data) {
    const executors = {};
    
    // Группируем операции по исполнителям
    data.forEach(item => {
        if (!executors[item.executor]) {
            executors[item.executor] = {
                id: "exec_" + item.executor,
                text: "Исполнитель " + item.executor,
                type: "project",
                open: true
            };
        }
    });
    
    // Формируем задачи
    const tasks = data.map(item => {
        const formatDate = (dateStr) => {
            const [date, time] = dateStr.split('_');
            const [dd, mm, yy] = date.split('.');
            const [hh, min, sec] = time.split(':');
            return new Date(2000 + +yy, mm - 1, dd, hh, min, sec);
        };
        
        return {
            id: "task_" + item.operation,
            text: "Операция " + item.operation,
            start_date: formatDate(item.start),
            end_date: formatDate(item.end),
            parent: "exec_" + item.executor,
            progress: 1,
            color: item.executor === 1 ? "#4682B4" : "#9370DB"
        };
    });
    
    return {
        data: [...Object.values(executors), ...tasks],
        links: []
    };
}

// Загрузка данных
gantt.parse(prepareData(scheduleData));

// Настройки отображения
gantt.config.scale_unit = "hour";
gantt.config.step = 1;
gantt.config.date_scale = "%H:%i";
gantt.config.subscales = [
    {unit: "minute", step: 30, date: "%H:%i"}
];

// Включаем группировку по проектам (исполнителям)
gantt.config.group_by = "parent";
gantt.config.order_branch = true;
gantt.config.order_branch_free = true;

// Кастомизация отображения задач
gantt.templates.task_text = function(start, end, task) {
    return task.text;
};

gantt.templates.task_class = function(start, end, task) {
    return task.type === "project" ? "project_task" : "";
};

// Обновляем отображение
gantt.render();