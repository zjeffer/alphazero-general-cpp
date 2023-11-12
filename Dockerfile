FROM zjeffer/docker-arch-cuda:main

WORKDIR /app

RUN pacman -Syyu --noconfirm clang cmake make gcc gdb git sudo less

# add user
################# USER & GROUP CREATION #################
ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME
################# SUDO #################
RUN echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME
USER $USERNAME
ENV HOME /home/$USERNAME

# g3log
COPY ./vendor/g3log/ ./vendor/g3log/
RUN sudo chown -R $USERNAME:$USERNAME /app
RUN mkdir /app/vendor/g3log/build
RUN cd /app/vendor/g3log/build && cmake .. && sudo cmake --build . --target install

# zsh & oh-my-zsh
RUN sudo pacman -S --noconfirm zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
RUN sudo chsh -s /bin/zsh $USERNAME

COPY . .