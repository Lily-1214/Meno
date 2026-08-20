#!/usr/bin/env python3
"""공개 헤더에 SFML이 새지 않았는지 검사한다.

UML 설계 초안 2-3절이 W1 산출물로 지정한 스크립트다.
기획서와 UML의 핵심 요구사항(P3: 의존성 은닉)을 사람이 아니라 CI가 지키게 한다.

    python tools/check_headers.py            # 저장소 루트에서
    python tools/check_headers.py --root .   # 경로 지정

위반이 있으면 1을 반환하므로 GitHub Actions 스텝으로 그대로 쓸 수 있다.

검사 대상은 include/ 아래 헤더뿐이다. src/ 아래는 사용자에게 배포되지 않으므로
SFML을 마음껏 써도 된다 -- 특히 src/meno/sfml_backend/ 는 그러라고 있는 곳이다.
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

# 공개 헤더에서 발견되면 안 되는 패턴.
#
# 주석은 제외한다. "이 헤더는 SFML을 include하지 않는다" 같은 설명 주석까지
# 걸리면 검사가 쓸모없어지기 때문이다. 실제로 그렇게 넓게 잡았다가 6건이 전부
# 주석이었던 적이 있다.
PATTERNS: list[tuple[str, re.Pattern[str]]] = [
    ("SFML 헤더 include", re.compile(r"^\s*#\s*include\s*[<\"]SFML/")),
    ("sf:: 심볼 사용", re.compile(r"\bsf::")),
]

COMMENT = re.compile(r"^\s*(//|/\*|\*)")

HEADER_SUFFIXES = {".hpp", ".h", ".hh", ".hxx", ".inl"}


def scan(path: Path) -> list[tuple[int, str, str]]:
    """파일 하나를 검사해 (줄번호, 위반 종류, 줄 내용) 목록을 돌려준다."""
    violations: list[tuple[int, str, str]] = []
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        text = path.read_text(encoding="utf-8", errors="replace")

    in_block_comment = False
    for lineno, line in enumerate(text.splitlines(), start=1):
        stripped = line.strip()

        # 여러 줄 주석 건너뛰기
        if in_block_comment:
            if "*/" in stripped:
                in_block_comment = False
            continue
        if stripped.startswith("/*") and "*/" not in stripped:
            in_block_comment = True
            continue
        if COMMENT.match(line):
            continue

        for label, pattern in PATTERNS:
            if pattern.search(line):
                violations.append((lineno, label, stripped))

    return violations


def main() -> int:
    # Windows 콘솔 기본 코드페이지(cp949)에서 한글 출력이 깨지지 않게 한다.
    # GitHub Actions는 UTF-8이라 영향이 없지만, 팀원이 로컬에서 돌릴 때 필요하다.
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8")
        except (AttributeError, ValueError):
            pass

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", default=".", help="저장소 루트 (기본: 현재 디렉터리)")
    args = parser.parse_args()

    include_dir = Path(args.root) / "include"
    if not include_dir.is_dir():
        print(f"[ERROR] include 디렉터리를 찾을 수 없다: {include_dir}", file=sys.stderr)
        return 2

    headers = sorted(p for p in include_dir.rglob("*") if p.suffix in HEADER_SUFFIXES)
    if not headers:
        print(f"[ERROR] {include_dir} 아래에 헤더가 없다", file=sys.stderr)
        return 2

    total = 0
    for header in headers:
        for lineno, label, line in scan(header):
            rel = header.relative_to(args.root).as_posix()
            print(f"{rel}:{lineno}: {label}: {line}")
            total += 1

    print()
    if total:
        print(f"[FAIL] 공개 헤더에서 SFML 유출 {total}건 (검사한 헤더 {len(headers)}개)")
        print("       구현은 src/ 로 옮기고, 공개 헤더에서는 pimpl로 감출 것.")
        return 1

    print(f"[PASS] 공개 헤더 {len(headers)}개, SFML 유출 없음")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
