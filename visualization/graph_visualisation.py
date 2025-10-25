import streamlit as st
import json
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from datetime import datetime
import plotly.subplots as sp

def parse_datetime(date_str):
    """Парсинг даты из формата dd.mm.yy_HH:MM:SS"""
    return datetime.strptime(date_str, '%d.%m.%y_%H:%M:%S')

def load_data(uploaded_file):
    """Загрузка и парсинг JSON файла"""
    try:
        data = json.load(uploaded_file)
        return data
    except Exception as e:
        st.error(f"Ошибка загрузки файла: {e}")
        return None

def create_gantt_chart(data):
    """Создание графика Ганта с правильным позиционированием"""
    gantt_data = []
    
    operation_to_work = {}
    for work in data['works']:
        for operation_id in work['operations']:
            operation_to_work[operation_id] = work['id']

    for tool_group in data['tools']:
        tool = tool_group
        tool_id = tool['id']
            
        for work_process in tool['work_process']:
            operation_id = work_process['operation']
            work_id = operation_to_work.get(operation_id, -1)
                
            start_time = parse_datetime(work_process['start'])
            end_time = parse_datetime(work_process['end'])
                
            operation_info = next((op for op in data['operations'] if op['id'] == operation_id), None)
                
            if operation_info:
                gantt_data.append({
                    'Tool': f'Tool {tool_id}',
                    'Operation': f'{operation_id}',
                    'Work': f'{work_id}',
                    'Start': start_time,
                    'Finish': end_time,
                    'Duration (s)': (end_time - start_time).total_seconds(),
                    'Appointed': 'True' if operation_info['appointed'] else 'False',
                    'Tool_Num': tool_id  # Числовой идентификатор для правильного порядка
                })
        for schedule in tool['shedule']:
            gantt_data.append({
                'Tool': f'Tool {tool_id}',
                'Start': parse_datetime(schedule['start']),
                'Finish': parse_datetime(schedule['end']),
                'Operation': 'Schedule',
                'Work': 'Schedule',
                'Color': 'lightgray',
                'Type': 'Schedule'
            })
    
    df = pd.DataFrame(gantt_data)
    
    if df.empty:
        st.warning("Нет данных для построения графика")
        return

    # Сортируем данные по инструментам
    df = df.sort_values('Tool_Num')
    
    # Создаем график с явным указанием категорий
    fig = px.timeline(
        df, 
        x_start="Start", 
        x_end="Finish", 
        y="Tool",
        color="Work",
        hover_data=["Operation", "Appointed", "Duration (s)"],
        title="График Ганта: Выполнение операций"
    )
    
    for trace in fig.data:
        if 'Schedule' in trace.name:
            trace.opacity = 0.1

    
    # Явно задаем порядок категорий на оси Y
    tool_order = sorted(df['Tool'].unique(), 
                       key=lambda x: int(x.replace('Tool ', '')))
    
    fig.update_layout(
        height=600,
        xaxis_title="Время",
        yaxis_title="Инструменты",
        showlegend=True,
        yaxis=dict(
            categoryorder='array',
            categoryarray=tool_order
        )
    )
    
    # Убеждаемся, что каждый бар занимает свою строку
    fig.update_yaxes(type='category')
    
    st.plotly_chart(fig, use_container_width=True)

def display_score(data):
    if 'score' in data and data['score']:
        score = data['score'][0]
        st.subheader("Штраф")
        st.metric(
            label="Штраф за назначенные операции в работах",
            value=f"{score.get('appointed_fine', 0):.6f}"
        )
        st.metric(
            label="Штраф за неназначенные операции", 
            value=f"{score.get('not_appointed_fine', 0):.6f}"
        )
        
def display_works_info(data):
    """Отображение информации о работах с выпадающим списком"""
    st.subheader("Информация о работах")
    
    if 'works' not in data or not data['works']:
        st.info("Нет информации о работах")
        return
    
    works_list = [f"Work {work['id']}" for work in data['works']] + ["Все работы"] 
    
    selected_work = st.selectbox(
        "Выберите работу для просмотра:",
        works_list,
        index=0
    )
    
    if selected_work == "Все работы":
        for work in data['works']:
            display_single_work(work, data)
    else:
        work_id = int(selected_work.replace("Work ", ""))
        selected_work_data = next((work for work in data['works'] if work['id'] == work_id), None)
        
        if selected_work_data:
            display_single_work(selected_work_data, data)
        else:
            st.error("Работа не найдена")

def style_dataframe(df):
    """Функция для стилизации DataFrame"""
    def highlight_appointed(row):
        if row['Назначена'] == 'Да' or row['Назначена'] == '✅ Да':
            return ['background-color: #90EE90'] * len(row)  # светло-зеленый
        else:
            return ['background-color: #FFB6C1'] * len(row)  # светло-красный

    styled_df = df.style.apply(highlight_appointed, axis=1)
    return styled_df

def display_single_work(work, data):
    with st.container():
        st.markdown(f"### Work {work['id']}")
        col1, col2, col3 = st.columns(3)
        
        with col1:
            st.metric("Время возможного начала", work['start_time'])
            st.metric("Коэффициент штрафа", f"{work['fine_coef']:.4f}")
            
        with col2:
            st.metric("Директивное время", work['directive'])
            
        with col3:
            appointed_ops = []
            not_appointed_ops = []
            
            for op_id in work['operations']:
                operation = next((op for op in data['operations'] if op['id'] == op_id), None)
                if operation:
                    if operation['appointed']:
                        appointed_ops.append(op_id)
            
            st.metric("Всего операций", len(work['operations']))
            st.metric("Назначенные операции", len(appointed_ops))
        
        st.subheader("Операции работы")
        
        operations_data = []
        for op_id in work['operations']:
            operation = next((op for op in data['operations'] if op['id'] == op_id), None)
            if operation:
                operations_data.append({
                    'ID': op_id,
                    'Назначена': '✅ Да' if operation['appointed'] else '❌ Нет',
                    'Прерываема': '✅ Да' if operation['stoppable'] else '❌ Нет',
                    'Начало': operation['start'],
                    'Конец': operation['end'],
                    'Потомки': str(operation['depended']) if operation['depended'] else 'Нет'
                })
        
        if operations_data:
            operations_df = pd.DataFrame(operations_data)
            st.dataframe(style_dataframe(operations_df), use_container_width=True)
        else:
            st.info("Нет информации об операциях")
        
        # Информация о выполнении на инструментах
        st.subheader("Выполнение на инструментах")
        
        execution_data = []
        for tool in data['tools']:
            for process in tool['work_process']:
                if process['operation'] in work['operations']:
                    execution_data.append({
                        'Инструмент': f"Tool {tool['id']}",
                        'Операция': process['operation'],
                        'Начало': process['start'],
                        'Конец': process['end'],
                        'Длительность (сек)': (parse_datetime(process['end']) - parse_datetime(process['start'])).total_seconds()
                    })
        
        if execution_data:
            execution_df = pd.DataFrame(execution_data)
            st.dataframe(execution_df, use_container_width=True)
        else:
            st.info("Операции этой работы не выполнялись на инструментах")
        
        st.markdown("---")

def display_operations_info(data):
    """Отображение информации об операциях"""
    st.subheader("Информация об операциях")
    
    if 'operations' not in data or not data['operations']:
        st.info("Нет информации об операциях")
        return
    
    operations_df = pd.DataFrame(data['operations'])
    
    # Добавляем информацию о работе для каждой операции
    work_mapping = {}
    for work in data.get('works', []):
        for op_id in work['operations']:
            work_mapping[op_id] = work['id']
    
    operations_df['Work'] = operations_df['id'].map(work_mapping)
    operations_df['Work'] = operations_df['Work'].fillna('Не назначена')
    
    # Форматируем данные для отображения
    display_df = operations_df[[
        'id', 'appointed', 'stoppable', 'start', 'end', 'Work'
    ]].copy()
    
    display_df.columns = ['ID', 'Назначена', 'Прерываема', 'Начало', 'Конец', 'Работа']
    display_df['Назначена'] = display_df['Назначена'].map({True: 'Да', False: 'Нет'})
    display_df['Прерываема'] = display_df['Прерываема'].map({True: 'Да', False: 'Нет'})
    
    st.dataframe(style_dataframe(display_df), use_container_width=True)

def main():
    st.set_page_config(
        page_title="Визуализатор задач промышленного планирования",
        page_icon="📊",
        layout="wide"
    )
    
    st.title("📊 Визуализатор задач промышленного планирования")
    
    # Загрузка файла
    uploaded_file = st.file_uploader(
        "Загрузите JSON файл с данными планирования", 
        type=['json']
    )
    
    if uploaded_file is not None:
        data = load_data(uploaded_file)
        if data:
            display_analysis(data)
            
def display_analysis(data):
    display_score(data)
    st.markdown("---")
    st.subheader("График Ганта выполнения операций")
    create_gantt_chart(data)
    st.markdown("---")
    display_works_info(data)
    st.markdown("---")
    display_operations_info(data)

if __name__ == "__main__":
    main()