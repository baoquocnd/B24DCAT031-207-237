# HƯỚNG DẪN SỬ DỤNG APP LÝ THUYẾT THÔNG TIN - GF(2)

Chương trình hỗ trợ tính toán các phép toán đại số trên trường hữu hạn GF(2) (trường Galois nhị phân), thường được ứng dụng trong Lý thuyết thông tin, Mã hóa và Mật mã học.

---

## 1. QUY ƯỚC BIỂU DIỄN ĐA THỨC

Mỗi chuỗi bit nhị phân được hiểu là các hệ số của một đa thức trên trường GF(2), với bit ngoài cùng bên phải tương ứng với hệ số bậc 0 ($x^0$).

**Ví dụ:**
* Chuỗi bit `1011` biểu diễn đa thức: $1 \cdot x^3 + 0 \cdot x^2 + 1 \cdot x^1 + 1 \cdot x^0 = x^3 + x + 1$
* Chuỗi bit `101` biểu diễn đa thức: $x^2 + 1$
* Chuỗi bit `11` biểu diễn đa thức: $x + 1$

---

## 2. LUỒNG HOẠT ĐỘNG CỦA CHƯƠNG TRÌNH

Chương trình hoạt động theo 2 bước chính:

### Bước 1: Chọn chế độ hiển thị
* **1. DAPAN (Đáp án):** Chương trình chỉ xuất ra kết quả cuối cùng ở dạng chuỗi bit. Phù hợp để chấm điểm tự động hoặc tra cứu nhanh.
* **2. BTTL (Bài tập tự luận):** Chương trình hiển thị chi tiết từng bước giải:
  * Chuyển đổi từ chuỗi bit sang dạng đa thức toán học.
  * Hiển thị phép toán và kết quả dạng đa thức.
  * Kết luận ngược lại ra chuỗi bit.

### Bước 2: Chọn hình thức nhập dữ liệu
* **1. Nhap tu FILE:** Đọc các lệnh tuần tự từ một file văn bản (ví dụ: `input.txt`).
* **2. Nhap tu BAN PHIM:** Cho phép người dùng gõ lệnh trực tiếp trên terminal và nhận kết quả ngay lập tức. Nhập `EXIT` hoặc `0` để thoát.

---

## 3. CÁC CÚ PHÁP HỖ TRỢ

| Cú pháp | Ý nghĩa | Ví dụ | Kết quả toán học |
| :--- | :--- | :--- | :--- |
| **`ADD <a> <b>`** | Phép cộng hai đa thức (tương đương phép XOR) | `ADD 1011 1101` | $(x^3+x+1) + (x^3+x^2+1) = x^2+x$ |
| **`MUL <a> <b>`** | Phép nhân hai đa thức | `MUL 101 11` | $(x^2+1) \cdot (x+1) = x^3+x^2+x+1$ |
| **`DIV <a> <b>`** | Phép chia lấy phần thương | `DIV 1111 101` | $(x^3+x^2+x+1) / (x^2+1) = x+1$ |
| **`MOD <a> <b>`** | Phép chia lấy phần dư | `MOD 1111 101` | $(x^3+x^2+x+1) \pmod{x^2+1} = 0$ |
| **`IRREDUCIBLE <a>`** | Kiểm tra đa thức tối giản (Bất khả quy) | `IRREDUCIBLE 1011` | `YES` (Đa thức $x^3+x+1$ tối giản) |

---

## 4. MẪU FILE INPUT CHUẨN (`input.txt`)

Nếu chọn chế độ đọc từ file, cấu trúc file input cần cấu tạo mỗi dòng một lệnh theo đúng cú pháp:

```text
ADD 1011 1101
MUL 101 11
DIV 1111 101
MOD 1111 101
IRREDUCIBLE 1011
IRREDUCIBLE 1001