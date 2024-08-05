import threading
import requests
import sys
import time

def make_request(url, index):
    start_time = time.time()
    try:
        response = requests.get(url)
        duration = time.time() - start_time
        print(f"Thread {index}: Response code: {response.status_code}, Time taken: {duration:.4f} seconds")
    except Exception as e:
        duration = time.time() - start_time
        print(f"Thread {index}: Request failed: {e}, Time taken: {duration:.4f} seconds")

#avec les thread ca marche pas ahaha
# def main(url, num_clients):
#     threads = []
#     total_start_time = time.time()
    
#     for i in range(num_clients):
#         thread = threading.Thread(target=make_request, args=(url, i))
#         threads.append(thread)
#         thread.start()
#       #   time.sleep(0.001)

#     for thread in threads:
#         thread.join()
    
#     total_duration = time.time() - total_start_time
#     print(f"\nTotal time taken: {total_duration:.4f} seconds")

def main(url, num_clients):
    threads = []
    total_start_time = time.time()
    
    for i in range(num_clients):
        make_request(url, i)
    
    total_duration = time.time() - total_start_time
    print(f"\nTotal time taken: {total_duration:.4f} seconds")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python test_script.py <url> <num_clients>")
        sys.exit(1)

    url = sys.argv[1]
    num_clients = int(sys.argv[2])
    main(url, num_clients)
