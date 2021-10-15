# grocery-beep

## Scanner



## Backend

### Docker build

```
cd backend
docker build . -t grocery-beep-be
docker run docker run -v $PWD/auth.json:/root/.config/gkeep/auth.json grocery-beep-be
```