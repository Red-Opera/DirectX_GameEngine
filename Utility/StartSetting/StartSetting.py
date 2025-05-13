from pathlib import Path
import re

# 병합할 .part01 경로들
FILES = [
    'External/physX/physX/libs/release/PhysXNarrowphaseGpu_static_64.lib.part01',
    'External/physX/physX/libs/debug/PhysXNarrowphaseGpu_static_64.lib.part01',
    'External/physX/physX/libs/release/PhysXGpu_64.dll.part01',
]

def merge_file(part01_path: Path) -> None:
    """
    .part01부터 시작하는 분할 파일들을 병합하여 원본 파일을 복원하고,
    병합 후 .partNN 파일들을 삭제합니다.
    """
    if not part01_path.exists():
        print(f"❌ 병합할 파트 파일이 없습니다: {part01_path}")
        return

    # 병합 대상 파일 이름 구하기 (예: "xxx.lib")
    merged_file_name = re.sub(r'\.part\d{2}$', '', part01_path.name)
    merged_path = part01_path.with_name(merged_file_name)

    # 병합 수행
    with merged_path.open('wb') as merged_file:
        part_number = 1
        while True:
            part_file = part01_path.with_name(f"{merged_file_name}.part{part_number:02d}")
            if not part_file.exists():
                break
            with part_file.open('rb') as pf:
                data = pf.read()
                merged_file.write(data)
                print(f"📦 병합 중: {part_file.name} ({len(data)}바이트)")
            part_number += 1

    print(f"🎉 병합 완료: {merged_path.name} ({merged_path.stat().st_size}바이트)")

    # 병합에 사용된 .part 파일 삭제
    deleted = 0
    for i in range(1, part_number):
        part_file = part01_path.with_name(f"{merged_file_name}.part{i:02d}")
        if part_file.exists():
            part_file.unlink()
            print(f"🗑️ 삭제됨: {part_file.name}")
            deleted += 1

    print(f"✅ 총 {deleted}개 파트 파일 삭제 완료\n")

if __name__ == '__main__':
    base_dir = Path(__file__).parent.parent.parent

    for rel in FILES:
        part01 = (base_dir / rel).resolve()
        merge_file(part01)
