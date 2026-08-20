# meno

SFML 기반 2D 게임 개발 라이브러리. 서울과학기술대학교 캡스톤디자인(2) 프로젝트.

게임 루프·객체 관리·충돌 판정·리소스 관리처럼 게임마다 반복 구현되는 기능을
상위 라이브러리로 제공한다. **사용자는 SFML을 직접 다루지 않는다.**

```cpp
#include <meno/Meno.hpp>

int main() {
    meno::Window window{{960u, 540u}, "내 게임"};
    meno::Renderer renderer{window};

    while (window.isOpen()) {
        window.pollEvents();
        renderer.beginFrame(meno::Color::fromHex(0x1E2430FF));
        renderer.drawCircle({480.f, 270.f}, 60.f, {.fill = meno::colors::Cyan});
        renderer.endFrame();
    }
}
```

## 빌드

요구 사항: **CMake 3.28+**, **C++20 컴파일러** (MSVC 2022 / GCC 13+ / Clang 16+)

SFML은 CMake가 자동으로 받아온다. 별도 설치가 필요 없다.

```bash
cmake -S . -B build
cmake --build build
```

> 첫 빌드는 10분쯤 걸린다. SFML과 그 의존성(freetype·harfbuzz·SheenBidi)을
> 함께 빌드하기 때문이다. 두 번째부터는 빠르다.

예제 실행:

```bash
./build/examples/meno_example_render_basics
```

테스트:

```bash
ctest --test-dir build --output-on-failure
```

## 디렉터리 구조

```
include/meno/     공개 헤더 — 여기에 SFML은 등장하지 않는다
  Meno.hpp          통합 헤더
  core/             Window, (Application, Clock)
  math/             Vec2, Rect, Color, Camera2D
  graphics/         Renderer, Texture, Font, DrawParams
src/meno/         구현 — 사용자에게 배포되지 않는다
  sfml_backend/     SFML을 include하는 유일한 곳
examples/         데모 게임
tests/            테스트
tools/            check_headers.py — 공개 헤더 SFML 유출 검사
docs/             설계 문서
```

`include/`와 `src/`가 나뉜 게 핵심이다. CMake에서 `include/`는 `PUBLIC`,
`src/`는 `PRIVATE`으로 지정해 **SFML이 사용자 빌드로 새지 않게** 한다.
자세한 근거는 [docs/render-api.md](docs/render-api.md)에 있다.

검사는 자동화돼 있다:

```bash
python tools/check_headers.py
```

## 문서

| 문서 | 내용 |
|---|---|
| [docs/render-api.md](docs/render-api.md) | 2D 렌더 API 설계 근거, 담당자 간 계약, SFML 3 변경점 |
| [docs/cpp-guide.md](docs/cpp-guide.md) | 이 코드로 배우는 현업 C++ — pimpl·RAII·소유권 |

## 개발

브랜치는 기능 단위로 파고 PR로 병합한다.

| 브랜치 | 담당 영역 |
|---|---|
| `2D_Renderer` | `graphics/`, `math/`, `sfml_backend/` |
| `GameObject-Demo` | `scene/`, `collision/` |

`math/`는 렌더러와 충돌 판정이 함께 쓰는 공용 타입이라, 변경 시 양쪽 리뷰를 거친다.

코드 스타일은 `.clang-format`이 강제한다. 저장 시 자동 포맷을 켜두면 된다.

## 라이선스

[zlib](LICENSE) — 의존하는 SFML과 동일하다.
