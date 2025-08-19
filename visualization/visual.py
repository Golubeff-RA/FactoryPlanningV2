import plotly.express as px
from datetime import datetime

# Ваши данные
data = [
    {"end": "17.08.25_12:00:00", "executor": 1, "operation": 101, "start": "17.08.25_10:00:00"},
    {"end": "17.08.25_15:00:00", "executor": 1, "operation": 102, "start": "17.08.25_13:00:00"},
    {"end": "17.08.25_11:30:00", "executor": 2, "operation": 201, "start": "17.08.25_09:00:00"}
]

# Преобразование дат
def parse_date(date_str):
    return datetime.strptime(date_str, "%d.%m.%y_%H:%M:%S")

# Подготовка данных для Plotly
df = []
for task in data:
    df.append({
        "Task": f"Операция {task['operation']}",
        "Start": parse_date(task["start"]),
        "Finish": parse_date(task["end"]),
        "Executor": f"Исполнитель {task['executor']}",
        "Operation": task["operation"]
    })

# Создание графика Ганта
fig = px.timeline(
    df,
    x_start="Start",
    x_end="Finish",
    y="Executor",
    color="Executor",
    text="Task",
    title="График выполнения операций",
    labels={"Executor": "Исполнитель"},
    color_discrete_sequence=px.colors.qualitative.Pastel
)

# Настройка внешнего вида
fig.update_traces(textposition="inside")
fig.update_yaxes(autorange="reversed")  # Исполнитель 1 вверху
fig.update_layout(
    hovermode="x",
    showlegend=True,
    xaxis_title="Время",
    yaxis_title="Исполнитель",
    height=400,
    width=800
)

# Настройка отображения времени
fig.update_xaxes(
    tickformat="%H:%M\n%d.%m.%y",
    rangeslider_visible=True
)

fig.show()