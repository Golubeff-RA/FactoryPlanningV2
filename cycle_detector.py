import sys
from collections import defaultdict, deque

def read_edges_from_stdin():
    """Читает список ребер из стандартного ввода"""
    edges = []
    for line in sys.stdin:
        line = line.strip()
        if not line:  # Пропускаем пустые строки
            continue
        parts = line.split()
        if len(parts) != 2:
            print(f"Пропускаю некорректную строку: {line}", file=sys.stderr)
            continue
        try:
            u, v = map(int, parts)
            edges.append((u, v))
        except ValueError:
            print(f"Пропускаю некорректную строку: {line}", file=sys.stderr)
    return edges

def has_cycle_dfs(edges):
    """Проверяет наличие циклов с помощью DFS"""
    graph = defaultdict(list)
    for u, v in edges:
        graph[u].append(v)
    
    # Состояния: 0 - не посещена, 1 - в обработке, 2 - обработана
    state = {}
    
    def dfs(node):
        if state.get(node) == 1:  # Найден цикл
            return True
        if state.get(node) == 2:  # Уже обработана
            return False
        
        state[node] = 1  # Помечаем как посещаемую
        
        for neighbor in graph.get(node, []):
            if dfs(neighbor):
                return True
        
        state[node] = 2  # Помечаем как обработанную
        return False
    
    # Получаем все вершины
    all_nodes = set()
    for u, v in edges:
        all_nodes.add(u)
        all_nodes.add(v)
    
    # Проверяем все компоненты связности
    for node in all_nodes:
        if node not in state:
            if dfs(node):
                return True
    
    return False

def has_cycle_kahn(edges):
    """Проверяет наличие циклов с помощью алгоритма Кана"""
    graph = defaultdict(list)
    in_degree = defaultdict(int)
    
    # Строим граф и подсчитываем входящие степени
    for u, v in edges:
        graph[u].append(v)
        in_degree[v] += 1
        if u not in in_degree:
            in_degree[u] = 0
    
    # Очередь вершин с нулевой входящей степенью
    queue = deque([node for node in in_degree if in_degree[node] == 0])
    processed = 0
    
    while queue:
        node = queue.popleft()
        processed += 1
        
        for neighbor in graph.get(node, []):
            in_degree[neighbor] -= 1
            if in_degree[neighbor] == 0:
                queue.append(neighbor)
    
    # Если обработаны не все вершины - есть цикл
    return processed != len(in_degree)

def main():
    print("Введите список ребер (каждое ребро на отдельной строке, формат: u v)")
    print("Завершите ввод Ctrl+D (Linux/Mac) или Ctrl+Z (Windows)")
    print()
    
    edges = read_edges_from_stdin()
    
    if not edges:
        print("Ошибка: не введено ни одного ребра", file=sys.stderr)
        sys.exit(1)
    
    print(f"Прочитано {len(edges)} ребер")
    print("Ребра:", edges)
    
    # Проверяем двумя алгоритмами для надежности
    result_dfs = has_cycle_dfs(edges)
    result_kahn = has_cycle_kahn(edges)
    
    print("\nРезультаты проверки:")
    print(f"Алгоритм DFS: {'ЦИКЛ ОБНАРУЖЕН' if result_dfs else 'ЦИКЛОВ НЕТ'}")
    print(f"Алгоритм Кана: {'ЦИКЛ ОБНАРУЖЕН' if result_kahn else 'ЦИКЛОВ НЕТ'}")
    
    if result_dfs != result_kahn:
        print("\nВНИМАНИЕ: Алгоритмы дали разные результаты!", file=sys.stderr)
        sys.exit(1)
    
    if result_dfs:
        print("\nГраф содержит как минимум один цикл")
        sys.exit(0)
    else:
        print("\nГраф ациклический")
        sys.exit(0)

if __name__ == "__main__":
    main()